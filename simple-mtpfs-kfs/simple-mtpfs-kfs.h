/* ***** BEGIN LICENSE BLOCK *****
*   Copyright (C) 2012-2016, Peter Hatina <phatina@gmail.com>
*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU General Public License as
*   published by the Free Software Foundation; either version 2 of
*   the License, or (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program. If not, see <http://www.gnu.org/licenses/>.
* ***** END LICENSE BLOCK ***** */

#ifndef SMTPFS_FUSE_H
#define SMTPFS_FUSE_H

#include <memory>
#include <string>
#include <cstdlib>
#include <sys/syslimits.h>
extern "C" {
#  include <KFS/KFS.h>
}

#include "simple-mtpfs-mtp-device.h"
#include "simple-mtpfs-tmp-files-pool.h"
#include "simple-mtpfs-type-tmp-file.h"

#define SMTPFS_MOVE_BY_SET_OBJECT_PROPERTY
#define HAVE_LIBMTP_CHECK_CAPABILITY

struct SMTPcontext_t {
    void *fs;
};

extern pthread_mutex_t control_mtx;
extern pthread_cond_t control_cv;
extern int in_fs;

class SMTPFileSystem
{
private:
    struct SMTPFileSystemOptions {
    public:
        int m_good;
        int m_help;
        int m_version;
        int m_verbose;
        int m_enable_move;
        int m_list_devices;
        int m_device_no;
        char *m_device_file;
        char *m_mount_point;

        SMTPFileSystemOptions();
        ~SMTPFileSystemOptions();

        static int opt_proc(void *data, const char *arg, int key,
            struct fuse_args *outargs);
    };

    enum {
        KEY_ENABLE_MOVE,
        KEY_DEVICE_NO,
        KEY_LIST_DEVICES,
        KEY_VERBOSE,
        KEY_VERSION,
        KEY_HELP
    };

public:
    SMTPFileSystem();
    ~SMTPFileSystem();

    bool parseOptions(int argc, char **argv);
    void printHelp(std::string progname) const;
    void printVersion() const;
    bool listDevices() const;

    bool isHelp() const { return m_options.m_help; }
    bool isVersion() const { return m_options.m_version; }
    bool isListDevices() const { return m_options.m_list_devices; }
    kfsfilesystem_t getFS() const { return m_kfs_filesystem; }
    const TypeDir* opendir(const char *path);
    
    bool mount(SMTPcontext_t *ctx);

    int symlink(const char *path, const char *value, int *error, SMTPcontext_t *context);
    int readlink(const char *path, char **value, int *error, SMTPcontext_t *context);
    int chmod(const char *path, kfsmode_t mode, int *error, SMTPcontext_t *context);
    int getattr(const char *path, kfsstat_t *result, int *error, SMTPcontext_t *context);
    int mkdir(const char *path, int *error, SMTPcontext_t *context);
    int unlink(const char *path, int *error, SMTPcontext_t *context);
    int rmdir(const char *path, int *error, SMTPcontext_t *context);
    int rename(const char *path, const char *new_path, int *error, SMTPcontext_t *context);
    int truncate(const char *path, uint64_t size, int *error, SMTPcontext_t *context);
    int utime(const char *path, const kfstime_t *atime, const kfstime_t *mtime, int *error, SMTPcontext_t *context);
    int read(const char *path, char *buf, size_t offset, size_t length, int *error, SMTPcontext_t *context);
    int write(const char *path, const char *buf, size_t offset, size_t length, int *error, SMTPcontext_t *context);
    int statfs(const char *path, kfsstatfs_t *result, int *error, SMTPcontext_t *context);
    int readdir(const char *path, kfscontents_t *contents, int *error, SMTPcontext_t *context);
    int create(const char *path, int *error, SMTPcontext_t *context);
    int remove(const char *path, int *error, SMTPcontext_t *context);
    int fsync(const char *path, int *error, SMTPcontext_t *context);
    int open(const char *path, int flags);
    int truncate(const char *path, off_t new_size);

private:
    bool hasPartialObjectSupport();

    kfsfilesystem_t m_kfs_filesystem;
    kfsid_t m_kfs_id;
    SMTPcontext_t m_kfs_context;
    TmpFilesPool m_tmp_files_pool;
    SMTPFileSystemOptions m_options;
    MTPDevice m_device;
    
};

#endif // SMTPFS_FUSE_H
