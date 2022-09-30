// This file is part of the AliceVision project.
// Copyright (c) 2022 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "filesystem.hpp"

namespace aliceVision {
namespace vfs {

namespace {

void throwIfFailedEc(error_code ec, const char* msg)
{
    if (ec)
    {
        throw boost::filesystem::filesystem_error(msg, ec);
    }
}

void throwIfFailedEc(error_code ec, const char* msg, const path& path)
{
    if (ec)
    {
        throw boost::filesystem::filesystem_error(msg, path.boost_path(), ec);
    }
}

void throwIfFailedEc(error_code ec, const char* msg, const path& path1, const path& path2)
{
    if (ec)
    {
        throw boost::filesystem::filesystem_error(msg, path1.boost_path(), path2.boost_path(), ec);
    }
}

} // namespace

bool is_virtual_path(const path& p)
{
    return false;
}

path absolute(const path& p)
{
    return boost::filesystem::absolute(p.boost_path());
}

path absolute(const path& p, const path& base)
{
    return boost::filesystem::absolute(p.boost_path(), base.boost_path());
}

path canonical(const path& p)
{
    return canonical(p, current_path());
}

path canonical(const path& p, error_code& ec)
{
    return canonical(p, current_path(), ec);
}

path canonical(const path& p, const path& base)
{
    error_code ec;
    auto result = canonical(p, base, ec);
    throwIfFailedEc(ec, "canonical", p, base);
    return result;
}

path canonical(const path& p, const path& base, error_code& ec)
{
    return boost::filesystem::canonical(p.boost_path(), base.boost_path(), ec);
}

void copy(const path& from, const path& to)
{
    error_code ec;
    copy(from, to, ec);
    throwIfFailedEc(ec, "copy", from, to);
}

void copy(const path& from, const path& to, error_code& ec)
{
    boost::filesystem::copy(from.boost_path(), to.boost_path(), ec);
}

void copy_directory(const path& from, const path& to)
{
    error_code ec;
    copy_directory(from, to, ec);
    throwIfFailedEc(ec, "copy_directory", from, to);
}

void copy_directory(const path& from, const path& to, error_code& ec)
{
    boost::filesystem::copy_directory(from.boost_path(), to.boost_path(), ec);
}

void copy_file(const path& from, const path& to)
{
    error_code ec;
    copy_file(from, to, ec);
    throwIfFailedEc(ec, "copy_file", from, to);
}

void copy_file(const path& from, const path& to, error_code& ec)
{
    boost::filesystem::copy_file(from.boost_path(), to.boost_path(), ec);
}

void copy_file(const path& from, const path& to, copy_options option)
{
    error_code ec;
    copy_file(from, to, option, ec);
    throwIfFailedEc(ec, "copy_file", from, to);
}

void copy_file(const path& from, const path& to, copy_options options, error_code& ec)
{
    boost::filesystem::copy_file(from.boost_path(), to.boost_path(),
                                 boost::native_value(options), ec);
}

void copy_symlink(const path& existing_symlink, const path& new_symlink)
{
    error_code ec;
    copy_symlink(existing_symlink, new_symlink, ec);
    throwIfFailedEc(ec, "copy_symlink", existing_symlink, new_symlink);
}

void copy_symlink(const path& existing_symlink, const path& new_symlink, error_code& ec)
{
    boost::filesystem::copy_symlink(existing_symlink.boost_path(), new_symlink.boost_path(), ec);
}

bool create_directories(const path& p)
{
    error_code ec;
    auto result = create_directories(p, ec);
    throwIfFailedEc(ec, "create_directories", p);
    return result;
}

bool create_directories(const path& p, error_code& ec)
{
    return boost::filesystem::create_directories(p.boost_path(), ec);
}

bool create_directory(const path& p)
{
    error_code ec;
    auto result = create_directory(p, ec);
    throwIfFailedEc(ec, "create_directory", p);
    return result;
}

bool create_directory(const path& p, error_code& ec)
{
    return boost::filesystem::create_directory(p.boost_path(), ec);
}

void create_directory_symlink(const path& to, const path& new_symlink)
{
    error_code ec;
    create_directory_symlink(to, new_symlink, ec);
    throwIfFailedEc(ec, "create_directory_symlink", to, new_symlink);
}

void create_directory_symlink(const path& to, const path& new_symlink, error_code& ec)
{
    boost::filesystem::create_directory_symlink(to.boost_path(), new_symlink.boost_path(), ec);
}

void create_hard_link(const path& to, const path& new_hard_link)
{
    error_code ec;
    create_hard_link(to, new_hard_link, ec);
    throwIfFailedEc(ec, "create_hard_link", to, new_hard_link);
}

void create_hard_link(const path& to, const path& new_hard_link, error_code& ec)
{
    boost::filesystem::create_hard_link(to.boost_path(), new_hard_link.boost_path(), ec);
}

void create_symlink(const path& to, const path& new_symlink)
{
    error_code ec;
    create_symlink(to, new_symlink, ec);
    throwIfFailedEc(ec, "create_symlink", to, new_symlink);
}

void create_symlink(const path& to, const path& new_symlink, error_code& ec)
{
    boost::filesystem::create_symlink(to.boost_path(), new_symlink.boost_path(), ec);
}

path current_path()
{
    error_code ec;
    path result = current_path(ec);
    throwIfFailedEc(ec, "current_path");
    return result;
}

path current_path(error_code& ec)
{
    return boost::filesystem::current_path(ec);
}

void current_path(const path& p)
{
    error_code ec;
    current_path(p, ec);
    throwIfFailedEc(ec, "current_path", p);
}

void current_path(const path& p, error_code& ec)
{
    boost::filesystem::current_path(p.boost_path(), ec);
}

bool exists(file_status s) noexcept
{
    return boost::filesystem::exists(s);
}

bool exists(const path& p)
{
    return boost::filesystem::exists(p.boost_path());
}

bool exists(const path& p, error_code& ec)
{
    return boost::filesystem::exists(p.boost_path(), ec);
}

bool equivalent(const path& p1, const path& p2)
{
    return boost::filesystem::equivalent(p1.boost_path(), p2.boost_path());
}

bool equivalent(const path& p1, const path& p2, error_code& ec)
{
    return boost::filesystem::equivalent(p1.boost_path(), p2.boost_path(), ec);
}

std::uintmax_t file_size(const path& p)
{
    error_code ec;
    auto result = file_size(p, ec);
    throwIfFailedEc(ec, "file_size", p);
    return result;
}

std::uintmax_t file_size(const path& p, error_code& ec)
{
    return boost::filesystem::file_size(p.boost_path(), ec);
}

std::uintmax_t hard_link_count(const path& p)
{
    error_code ec;
    auto result = hard_link_count(p, ec);
    throwIfFailedEc(ec, "hard_link_count", p);
    return result;
}

std::uintmax_t hard_link_count(const path& p, error_code& ec)
{
    return boost::filesystem::hard_link_count(p.boost_path(), ec);
}

bool is_directory(file_status s) noexcept
{
    return boost::filesystem::is_directory(s);
}

bool is_directory(const path& p)
{
    return boost::filesystem::is_directory(p.boost_path());
}

bool is_directory(const path& p, error_code& ec)
{
    return boost::filesystem::is_directory(p.boost_path(), ec);
}

bool is_empty(const path& p)
{
    return boost::filesystem::is_empty(p.boost_path());
}

bool is_empty(const path& p, error_code& ec)
{
    return boost::filesystem::is_empty(p.boost_path(), ec);
}

bool is_other(file_status s) noexcept
{
    return boost::filesystem::is_other(s);
}

bool is_other(const path& p)
{
    return boost::filesystem::is_other(p.boost_path());
}

bool is_other(const path& p, error_code& ec)
{
    return boost::filesystem::is_other(p.boost_path(), ec);
}

bool is_regular_file(file_status s) noexcept
{
    return boost::filesystem::is_regular_file(s);
}

bool is_regular_file(const path& p)
{
    return boost::filesystem::is_regular_file(p.boost_path());
}

bool is_regular_file(const path& p, error_code& ec)
{
    return boost::filesystem::is_regular_file(p.boost_path(), ec);
}

bool is_symlink(file_status s)
{
    return boost::filesystem::is_symlink(s);
}

bool is_symlink(const path& p)
{
    return boost::filesystem::is_symlink(p.boost_path());
}

bool is_symlink(const path& p, error_code& ec)
{
    return boost::filesystem::is_symlink(p.boost_path(), ec);
}

std::time_t last_write_time(const path& p)
{
    error_code ec;
    auto result = last_write_time(p, ec);
    throwIfFailedEc(ec, "last_write_time", p);
    return result;
}

std::time_t last_write_time(const path& p, error_code& ec)
{
    return boost::filesystem::last_write_time(p.boost_path(), ec);
}

void last_write_time(const path& p, const std::time_t new_time)
{
    error_code ec;
    last_write_time(p, new_time, ec);
    throwIfFailedEc(ec, "last_write_time", p);
}

void last_write_time(const path& p, const std::time_t new_time, error_code& ec)
{
    boost::filesystem::last_write_time(p.boost_path(), new_time, ec);
}

path read_symlink(const path& p)
{
    error_code ec;
    auto result = read_symlink(p, ec);
    throwIfFailedEc(ec, "read_symlink", p);
    return result;
}

path read_symlink(const path& p, error_code& ec)
{
    return boost::filesystem::read_symlink(p.boost_path(), ec);
}

path relative(const path& p)
{
    error_code ec;
    auto result = relative(p, current_path(), ec);
    throwIfFailedEc(ec, "relative", p);
    return result;
}

path relative(const path& p, error_code& ec)
{
    return relative(p, current_path(), ec);
}

path relative(const path& p, const path& base)
{
    error_code ec;
    auto result = relative(p, base, ec);
    throwIfFailedEc(ec, "relative", p, base);
    return result;
}

path relative(const path& p, const path& base, error_code& ec)
{
    return boost::filesystem::relative(p.boost_path(), base.boost_path(), ec);
}

bool remove(const path& p)
{
    error_code ec;
    auto result = remove(p, ec);
    throwIfFailedEc(ec, "remove", p);
    return result;
}

bool remove(const path& p, error_code& ec)
{
    return boost::filesystem::remove(p.boost_path(), ec);
}

std::uintmax_t remove_all(const path& p)
{
    error_code ec;
    auto result = remove_all(p, ec);
    throwIfFailedEc(ec, "remove_all", p);
    return result;
}

std::uintmax_t remove_all(const path& p, error_code& ec)
{
    return boost::filesystem::remove_all(p.boost_path(), ec);
}

void rename(const path& from, const path& to)
{
    error_code ec;
    rename(from, to, ec);
    throwIfFailedEc(ec, "rename", from, to);
}

void rename(const path& from, const path& to, error_code& ec)
{
    boost::filesystem::rename(from.boost_path(), to.boost_path(), ec);
}

void resize_file(const path& p, std::uintmax_t size)
{
    error_code ec;
    resize_file(p, size, ec);
    throwIfFailedEc(ec, "resize_file", p);
}

void resize_file(const path& p, std::uintmax_t size, error_code& ec)
{
    boost::filesystem::resize_file(p.boost_path(), size, ec);
}

space_info space(const path& p)
{
    error_code ec;
    auto result = space(p, ec);
    throwIfFailedEc(ec, "space", p);
    return result;
}

space_info space(const path& p, error_code& ec)
{
    return boost::filesystem::space(p.boost_path(), ec);
}

file_status status(const path& p)
{
    return boost::filesystem::status(p.boost_path());
}

file_status status(const path& p, error_code& ec)
{
    return boost::filesystem::status(p.boost_path(), ec);
}

bool status_known(file_status s) noexcept
{
    return boost::filesystem::status_known(s);
}

file_status symlink_status(const path& p)
{
    return boost::filesystem::symlink_status(p.boost_path());
}

file_status symlink_status(const path& p, error_code& ec)
{
    return boost::filesystem::symlink_status(p.boost_path(), ec);
}

path system_complete(const path& p)
{
    error_code ec;
    auto result = system_complete(p, ec);
    throwIfFailedEc(ec, "system_complete", p);
    return result;
}

path system_complete(const path& p, error_code& ec)
{
    return boost::filesystem::system_complete(p.boost_path(), ec);
}

path temp_directory_path()
{
    error_code ec;
    auto result = temp_directory_path(ec);
    throwIfFailedEc(ec, "temp_directory_path");
    return result;
}

path temp_directory_path(error_code& ec)
{
    return boost::filesystem::temp_directory_path(ec);
}

path unique_path(const path& model)
{
    error_code ec;
    auto result = unique_path(model, ec);
    throwIfFailedEc(ec, "unique_path", model);
    return result;
}

path unique_path(const path& model, error_code& ec)
{
    return boost::filesystem::unique_path(model.boost_path(), ec);
}

path weakly_canonical(const path& p)
{
    error_code ec;
    auto result = weakly_canonical(p, ec);
    throwIfFailedEc(ec, "weakly_canonical", p);
    return result;
}

path weakly_canonical(const path& p, error_code& ec)
{
    return boost::filesystem::weakly_canonical(p.boost_path(), ec);
}

} //namespace vfs
} //namespace aliceVision
