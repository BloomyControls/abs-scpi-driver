# customize Doxygen settings
set(DOXYGEN_PROJECT_NAME "ABS SCPI Driver")
set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs)
set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
set(DOXYGEN_QT_AUTOBRIEF YES)
set(DOXYGEN_MULTILINE_CPP_IS_BRIEF YES)
set(DOXYGEN_TAB_SIZE 2)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
set(DOXYGEN_TYPEDEF_HIDES_STRUCT YES)
set(DOXYGEN_EXTRACT_LOCAL_CLASSES NO)
set(DOXYGEN_HIDE_SCOPE_NAMES YES)
set(DOXYGEN_SHOW_INCLUDE_FILES NO)
set(DOXYGEN_LAYOUT_FILE docs/DoxygenLayout.xml)
# set(DOXYGEN_INPUT include README.md)
# set(DOXYGEN_RECURSIVE YES)
set(DOXYGEN_EXCLUDE include/bci/abs/util/expected.hpp)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
set(DOXYGEN_VERBATIM_HEADERS NO)
set(DOXYGEN_HTML_HEADER docs/header.html)
set(DOXYGEN_HTML_EXTRA_STYLESHEET docs/doxygen-awesome-css/doxygen-awesome.css
  docs/doxygen-awesome-css/doxygen-awesome-sidebar-only.css
  docs/doxygen-awesome-css/doxygen-awesome-sidebar-only-darkmode-toggle.css
  docs/custom.css)
set(DOXYGEN_HTML_EXTRA_FILES docs/doxygen-awesome-css/doxygen-awesome-darkmode-toggle.js
  docs/doxygen-awesome-css/doxygen-awesome-paragraph-link.js
  docs/doxygen-awesome-css/doxygen-awesome-fragment-copy-button.js
  docs/doxygen-awesome-css/doxygen-awesome-interactive-toc.js)
set(DOXYGEN_HTML_COLORSTYLE LIGHT)
set(DOXYGEN_HTML_COLORSTYLE_HUE 209)
set(DOXYGEN_HTML_COLORSTYLE_SAT 255)
set(DOXYGEN_HTML_COLORSTYLE_GAMMA 113)
set(DOXYGEN_HTML_DYNAMIC_SECTIONS YES)
set(DOXYGEN_TOC_EXPAND YES)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_GENERATE_LATEX NO)
set(DOXYGEN_INCLUDE_PATH include)
set(DOXYGEN_QUIET YES)

doxygen_add_docs(absscpi-docs
  include README.md
  COMMENT "Generate documentation"
)
