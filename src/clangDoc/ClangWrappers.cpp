#include "ClangWrappers.hpp"

namespace clangw {
  TranslationUnit Index::makeTranslationUnit(
    const char* path,
    const char* const * argv, const int argc,
    CXUnsavedFile* unsavedFiles, const unsigned int unsavedFilesN,
    const unsigned int flags/* = CXTranslationUnit_None*/
  ) {
    return TranslationUnit(shared_from_this(), path, argv, argc, unsavedFiles, unsavedFilesN, flags);
  }

  const char* clangerr::what() const noexcept /*override*/ {
    return msg_;
  }
}
