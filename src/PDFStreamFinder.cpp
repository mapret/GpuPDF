#include "PDFStreamFinder.hpp"
#include "PDFDocument.hpp"
#include "math/Rectangle.hpp"

std::vector<PDFStreamFinder::GraphicsStream> PDFStreamFinder::GetGraphicsStreams(
  const std::filesystem::path& sourceFile) const
{
  PDFDocument document;
  document.Load(sourceFile);

  std::vector<PDFStreamFinder::GraphicsStream> streams;

  for (const auto& [objectId, pdfObject] : document.GetObjects())
  {
    if (pdfObject.IsDictionary() && pdfObject.GetDictionary().contains("MediaBox"))
    {
      const auto& mediaBoxArray{ pdfObject.GetDictionary().at("MediaBox").GetArray() };
      Rectangle mediaBox;
      mediaBox.min.x = static_cast<float>(mediaBoxArray[0].GetDecimalOrInt());
      mediaBox.min.y = static_cast<float>(mediaBoxArray[1].GetDecimalOrInt());
      mediaBox.max.x = static_cast<float>(mediaBoxArray[2].GetDecimalOrInt());
      mediaBox.max.y = static_cast<float>(mediaBoxArray[3].GetDecimalOrInt());

      if (pdfObject.GetDictionary().at("Type").GetName() == "Page")
      {
        auto AddToStream{ [&](const PDFObject& streamObjectReference)
        {
          const PDFObject& streamObject{ document.GetObjects().at(streamObjectReference.GetReference()) };
          streams.push_back(GraphicsStream{ streamObject.GetStream(), mediaBox });
        } };

        auto contents{ pdfObject.GetDictionary().at("Contents") };
        if (contents.IsReference())
        {
          AddToStream(contents);
        }
        else if (contents.IsArray())
        {
          for (auto& arrayEntry : contents.GetArray())
          {
            AddToStream(arrayEntry);
          }
        }
      }
    }
  }

  return streams;
}
