#include "PDFStreamFinder.hpp"
#include "math/Rectangle.hpp"
#include <podofo/podofo.h>

std::vector<PDFStreamFinder::GraphicsStream> PDFStreamFinder::GetGraphicsStreams(
  const std::filesystem::path& sourceFile) const
{
  PoDoFo::PdfMemDocument document;
  document.Load(sourceFile.string());

  std::vector<PDFStreamFinder::GraphicsStream> streams;

  for (const auto& pdfObject : document.GetObjects())
  {
    if (pdfObject->IsDictionary() && pdfObject->GetDictionary().HasKey("MediaBox"))
    {
      const auto& mediaBoxArray{ pdfObject->GetDictionary().MustGetKey("MediaBox").GetArray() };
      Rectangle mediaBox;
      mediaBox.min.x = mediaBoxArray[0].GetReal();
      mediaBox.min.y = mediaBoxArray[1].GetReal();
      mediaBox.max.x = mediaBoxArray[2].GetReal();
      mediaBox.max.y = mediaBoxArray[3].GetReal();

      if (pdfObject->GetDictionary().GetKey("Type")->GetName() == "Page")
      {
        auto AddToStream{ [&](PoDoFo::PdfObject& streamObject)
        {
          const auto& pdfStream{ document.GetObjects().MustGetObject(streamObject.GetReference()).MustGetStream() };
          PoDoFo::charbuff buffer;
          pdfStream.CopyToSafe(buffer);
          streams.push_back(GraphicsStream{ buffer, mediaBox });
        } };

        auto contents{ pdfObject->GetDictionary().GetKey("Contents") };
        if (contents->IsReference())
        {
          AddToStream(*contents);
        }
        else if (contents->IsArray())
        {
          for (auto& arrayEntry : contents->GetArray())
          {
            AddToStream(arrayEntry);
          }
        }
      }
    }
  }

  return streams;
}
