#include "PDFStreamFinder.hpp"
#include "math/Rectangle.hpp"
#include <podofo/podofo.h>

std::vector<PDFStreamFinder::GraphicsStream> PDFStreamFinder::GetGraphicsStreams(
  const std::filesystem::path& sourceFile) const
{
  PoDoFo::PdfMemDocument document;
  document.Load(sourceFile.string());

  std::vector<PDFStreamFinder::GraphicsStream> streams;

  for (const auto& a : document.GetObjects())
  {
    if (a->IsDictionary() && a->GetDictionary().HasKey("MediaBox"))
    {
      if (a->GetDictionary().GetKey("Type")->GetName() == "Page")
      {
        const auto& pdfStream{
          document.GetObjects().MustGetObject(a->GetDictionary().GetKey("Contents")->GetReference()).MustGetStream()
        };
        PoDoFo::charbuff buffer;
        pdfStream.CopyToSafe(buffer);

        const auto& array{ a->GetDictionary().MustGetKey("MediaBox").GetArray() };
        Rectangle mediaBox;
        mediaBox.min.x = array[0].GetReal();
        mediaBox.min.y = array[1].GetReal();
        mediaBox.max.x = array[2].GetReal();
        mediaBox.max.y = array[3].GetReal();

        streams.push_back(GraphicsStream{ buffer, mediaBox });
      }
    }
  }

  return streams;
}
