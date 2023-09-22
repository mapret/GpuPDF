//#include "PDFStreamReader.hpp"
//#include "PDFreader.hpp"
#include "Window.hpp"
//#include "math/Vector.hpp"
//#include <GLFW/glfw3.h>
//#include <iostream>
// #include <podofo/podofo.h>

/*
std::set<PoDoFo::PdfObject*> visitedObjects;

void Print(int indent, const std::string& name, PoDoFo::PdfDocument& document, PoDoFo::PdfObject& object)
{
  std::string indentString = std::string(indent * 2, ' ') + name + ":";

  if (visitedObjects.find(&object) != visitedObjects.end())
  {
    std::cout << indentString << "skip\n";
    return;
  }
  visitedObjects.insert(&object);

  if (object.IsDictionary())
  {
    std::cout << indentString << "Dictionary\n";
    for (auto& entry : object.GetDictionary())
    {
      //std::cout << indentString << "dict " << entry.first.GetString() << "\n";
      Print(indent + 1, entry.first.GetString(), document, entry.second);
    }
  }
  else if (object.IsNumber())
  {
    std::cout << indentString << "Number = " << object.GetNumber() << "\n";
  }
  else if (object.IsName())
  {
    std::cout << indentString << "Name = " << object.GetName().GetEscapedName() << "\n";
  }
  else if (object.IsReference())
  {
    std::cout << indentString << "Reference = " << object.GetReference().ObjectNumber() << "\n";
    Print(indent + 1, object.GetReference().ToString(), document,
document.GetObjects().MustGetObject(object.GetReference()));
  }
  else if (object.IsArray())
  {
    std::cout << indentString << "Array = " << object.GetArray().size() << "\n";
    int i = 0;
    for (auto& entry : object.GetArray())
    {
      Print(indent + 1, "[" + std::to_string(i) + "]", document, entry);
      i++;
    }
  }
  else
  {
    std::cout << indentString << "Other = " << object.GetDataTypeString() << "\n";
  }

  if (object.HasStream())
  {
    //indentString +=
    //    "<stream" + std::to_string(object.MustGetStream().GetLength()) + ">";
    PoDoFo::charbuff stream;
    object.MustGetStream().CopyToSafe(stream);
    // TODO: ???
    //PoDoFo::PdfCanvas canvas;
    //PoDoFo::PdfPainter painter;

    //PoDoFo::PdfContentStreamReader streamReader(object.MustGetStream())
    //PdfContentStreamOperators


    //PoDoFo::PdfContentStreamReader()
    //indentString += std::to_string(stream.size()) + ">";
    //std::cout << indentString << "stream=\n" << stream << "\nstream end\n";
  }
}*/

int main(int argc, char** argv)
{
  if (argc >= 1)
  {
    Window window;
    window.Run(argv[1]);

#if 0
    PoDoFo::PdfMemDocument document;
    document.Load(argv[1]);

    auto& page = document.GetPages().GetPageAt(0).MustGetContents();
    /*std::cout << page.GetObject().GetDataTypeString() << "\n";
    std::cout << page.GetObject().GetDictionary().size() << "\n";
    for(auto& a : page.GetObject().GetDictionary().GetIndirectIterator())
    {
      std::cout << "  " << a.second->GetDataTypeString() << "\n";
    }*/

    //Print(0, document, page.GetObject());
    //Print(0, "root", document, document.GetCatalog().GetObject());

    //std::cout << "objects " << document.GetObjects().GetSize() << "\n";
    for (const auto& a : document.GetObjects()) {
      //std::cout << a->GetDataTypeString() << "\n";
      if (a->IsDictionary() && a->GetDictionary().HasKey("MediaBox")) {
        /*std::cout << "media box: "
                  << a->GetDictionary().GetKey("Type")->GetName().GetString()
                  << "\n";*/
        if (a->GetDictionary().GetKey("Type")->GetName() == "Page") {
          // std::cout << "stream: " << a->GetDictionary().GetKey("Contents")->GetReference().->->GetLength() << "\n";
          auto &stream =
              document.GetObjects()
                  .MustGetObject(
                      a->GetDictionary().GetKey("Contents")->GetReference())
                  .MustGetStream();
          //std::cout << "stream: " << stream.GetLength() << "\n";
          PoDoFo::charbuff s;
          stream.CopyToSafe(s);
          //std::cout << "stream: " << s << "streamend\n";

          PdfStreamReader reader{ std::move(s) };
          reader.Read();
        }
      }
    }

    //for (const auto& a : document.GetPages().GetDictionary())
    //  std::cout << "dict " << a.first.GetString() << ", " << a.second.GetDataTypeString() << "\n";

    //https://github.com/podofo/podofo/blob/master/src/podofo/private/PdfDrawingOperations.cpp
    //FT_Outline_Decompose(nullptr, nullptr, nullptr);

    /*std::cout << "end 1\n";
    auto& a = document.GetPages().GetPageAt(0).MustGetResources();
    for (auto& b : a.GetDictionary())
    {
      std::cout << b.first.GetString() << ", " << b.second. << "\n";
    }
    std::cout << "asdf" << "\n";*/
#endif
  }

  return 0;
}
