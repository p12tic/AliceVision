# Headers
set(vfs_files_headers
    boost_common.hpp
    BoostDirectoryIteratorImpl.hpp
    directory_entry.hpp
    directory_iterator.hpp
    IFilesystemTree.hpp
    IDirectoryIteratorImpl.hpp
    filesystem.hpp
    FilesystemImplUtils.hpp
    FilesystemManager.hpp
    FilesystemTreeInMemory.hpp
    generic_filebuf.hpp
    fwd.hpp
    istream.hpp
    null_streambuf.hpp
    ostream.hpp
    path.hpp
    special_data.hpp
    std_filebuf.hpp
)

# Sources
set(vfs_files_sources
    BoostDirectoryIteratorImpl.cpp
    directory_entry.cpp
    directory_iterator.cpp
    IFilesystemTree.cpp
    IDirectoryIteratorImpl.cpp
    filesystem.cpp
    FilesystemManager.cpp
    FilesystemTreeInMemory.cpp
    istream.cpp
    ostream.cpp
    path.cpp
    special_data.cpp
    std_filebuf.cpp
)

alicevision_add_library(aliceVision_vfs
  SOURCES ${vfs_files_headers} ${vfs_files_sources}
  PUBLIC_LINKS
    Boost::filesystem
)

alicevision_add_test(filesystem_test.cpp NAME "filesystem" LINKS aliceVision_vfs)
