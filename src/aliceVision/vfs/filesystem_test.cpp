// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "filesystem.hpp"
#include "istream.hpp"
#include "ostream.hpp"
#include "FilesystemManager.hpp"
#include "FilesystemTreeInMemory.hpp"

#define BOOST_TEST_MODULE filesystem

#include <boost/test/unit_test.hpp>
#include <set>

using namespace aliceVision;

struct FilesystemTestFixture
{
    FilesystemTestFixture()
    {
        vfs::getManager().installTreeAtRoot("//test",
                                            std::make_unique<vfs::FilesystemTreeInMemory>());
        vfs::current_path("//test/");
    }

    ~FilesystemTestFixture()
    {
        vfs::getManager().clear();
    }
};

void createFile(const std::string& path, const std::string& contents)
{
    vfs::ostream outFile(path, std::ios_base::out);
    outFile << contents;
    outFile.close();
}

void checkFileContents(const std::string& path, const std::string& expectedContents)
{
    vfs::istream inFile(path, std::ios_base::in);
    std::vector<char> data;
    std::size_t readSize = 1024 * 128;
    while (true)
    {
        auto previousSize = data.size();
        data.resize(previousSize + readSize);
        inFile.read(data.data() + previousSize, readSize);
        data.resize(previousSize + inFile.gcount());
        if (inFile.gcount() == 0)
            break;
    }

    std::string contents(data.begin(), data.end());
    BOOST_CHECK_EQUAL(contents, expectedContents);
}

BOOST_FIXTURE_TEST_CASE(filesystem_create_file_read_by_line, FilesystemTestFixture)
{
    createFile("//test/file", "testdata\ntestdata2\n");
    vfs::istream inFile("//test/file", std::ios_base::in);
    std::string data;
    std::getline(inFile, data);
    BOOST_CHECK_EQUAL(data, "testdata");
    std::getline(inFile, data);
    BOOST_CHECK_EQUAL(data, "testdata2");
    BOOST_CHECK_EQUAL(inFile.eof(), false);
    std::getline(inFile, data);
    BOOST_CHECK_EQUAL(data, "");
    BOOST_CHECK_EQUAL(inFile.eof(), true);
}

BOOST_FIXTURE_TEST_CASE(filesystem_create_file, FilesystemTestFixture)
{
    createFile("//test/file", "testdata\ntestdata2\n");
    checkFileContents("//test/file", "testdata\ntestdata2\n");
}

BOOST_FIXTURE_TEST_CASE(filesystem_rename_file, FilesystemTestFixture)
{
    createFile("//test/file", "testdata\ntestdata2\n");
    vfs::rename("//test/file", "//test/file2");
    checkFileContents("//test/file2", "testdata\ntestdata2\n");
}

BOOST_FIXTURE_TEST_CASE(filesystem_create_directory, FilesystemTestFixture)
{
    vfs::create_directory("//test/dir");
    BOOST_CHECK_EQUAL(vfs::is_directory("//test/dir"), true);
}

BOOST_FIXTURE_TEST_CASE(filesystem_create_directory_slash, FilesystemTestFixture)
{
    vfs::create_directory("//test/dir/");
    BOOST_CHECK_EQUAL(vfs::is_directory("//test/dir"), true);
}

BOOST_FIXTURE_TEST_CASE(filesystem_create_directories, FilesystemTestFixture)
{
    vfs::create_directories("//test/a/b/c");
    BOOST_CHECK_EQUAL(vfs::is_directory("//test/"), true);
    BOOST_CHECK_EQUAL(vfs::is_directory("//test/a/"), true);
    BOOST_CHECK_EQUAL(vfs::is_directory("//test/a/b"), true);
    BOOST_CHECK_EQUAL(vfs::is_directory("//test/a/b/c"), true);
}

BOOST_FIXTURE_TEST_CASE(filesystem_rename_file_subdirectory, FilesystemTestFixture)
{
    createFile("//test/file", "testdata\ntestdata2\n");
    vfs::create_directory("//test/dir");
    vfs::rename("//test/file", "//test/dir/file2");
    checkFileContents("//test/dir/file2", "testdata\ntestdata2\n");
}

BOOST_FIXTURE_TEST_CASE(filesystem_create_special_data, FilesystemTestFixture)
{
    auto data = std::make_shared<vfs::special_data>();
    vfs::set_special_data("//test/fn", data);
    auto dataReceived = vfs::get_special_data("//test/fn");
    BOOST_CHECK_EQUAL(data.get(), dataReceived.get());
}

BOOST_FIXTURE_TEST_CASE(filesystem_create_special_data_subdir, FilesystemTestFixture)
{
    auto data = std::make_shared<vfs::special_data>();
    vfs::create_directory("//test/dir");
    vfs::set_special_data("//test/dir/fn", data);
    auto dataReceived = vfs::get_special_data("//test/dir/fn");
    BOOST_CHECK_EQUAL(data.get(), dataReceived.get());
}

BOOST_FIXTURE_TEST_CASE(filesystem_iterate_directory, FilesystemTestFixture)
{
    vfs::create_directory("//test/dir1");
    createFile("//test/file1", "testdata\ntestdata2\n");
    createFile("//test/file2", "");

    std::set<std::string> foundDirs;
    for (const auto& entry : vfs::directory_iterator("//test/"))
    {
        foundDirs.insert(entry.path().string());
    }

    std::set<std::string> expectedDirs = {
        "//test/dir1",
        "//test/file1",
        "//test/file2",
    };
    BOOST_CHECK(foundDirs == expectedDirs);
}

BOOST_FIXTURE_TEST_CASE(filesystem_iterate_subdirectory, FilesystemTestFixture)
{
    vfs::create_directory("//test/dir");
    vfs::create_directory("//test/dir/dir1");
    createFile("//test/dir/file1", "testdata\ntestdata2\n");
    createFile("//test/dir/file2", "");

    std::set<std::string> foundDirs;
    for (const auto& entry : vfs::directory_iterator("//test/dir"))
    {
        foundDirs.insert(entry.path().string());
    }

    std::set<std::string> expectedDirs = {
        "//test/dir/dir1",
        "//test/dir/file1",
        "//test/dir/file2",
    };
    BOOST_CHECK(foundDirs == expectedDirs);
}
