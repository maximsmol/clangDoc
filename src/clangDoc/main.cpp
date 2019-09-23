#include <iostream>
#include "ClangWrappers.hpp"

using namespace std;
using namespace clangw;

ostream& operator<<(ostream& stream, const CXString& str);

int main() {
  shared_ptr<clangw::Index> i = make_shared<Index>(false, true);
  TranslationUnit tu = i->makeTranslationUnit(
    "header.hpp",
    nullptr, 0,
    nullptr, 0
  );

  Cursor root = tu.getRootCursor();
  root.visitChildren([](Cursor c, Cursor, CXClientData) {
    if (!clang_Location_isFromMainFile(c.location()))
      return CXChildVisit_Continue;

    cout
      << c.spelling().cstr()
      << " (" << c.kind().spelling().cstr() << ")\n";
    return CXChildVisit_Recurse;
  });
}
