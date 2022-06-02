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

#include <iostream>
extern "C" {
#  include <errno.h>
#  include <unistd.h>
#  include <stddef.h>
#  include <fcntl.h>
#  include <getopt.h>
}
#include "simple-mtpfs-kfs.h"
#include "simple-mtpfs-log.h"
#include "simple-mtpfs-util.h"

#define PACKAGE_BUGREPORT "Nobody"

pthread_cond_t ready;

static void fs_in(){
    in_fs = 1;
    pthread_cond_signal(&control_cv);
}

static void fs_out(){
    in_fs = 0;
}

static bool wrap_getattr(const char *path, kfsstat_t *result, int *error, void *context)
{
    fs_in();
    SMTPcontext_t *ctx = (SMTPcontext_t*)context;
    SMTPFileSystem *fs = (SMTPFileSystem*)ctx->fs;
    int ret = fs->getattr(path, result, error, (SMTPcontext_t*)context);
    if(ret == 0){
        *error = 0;
        return true;
    }
    else {
        *error = ret;
        return false;
    }
}

static bool wrap_mkdir(const char *path, int *error, void *context)
{
    fs_in();
    SMTPcontext_t *ctx = (SMTPcontext_t*)context;
    int ret = ((SMTPFileSystem*)ctx->fs)->mkdir(path, error, (SMTPcontext_t*)context);
    if(ret == 0){
        *error = 0;
        return true;
    }
    else {
        *error = ret;
        return false;
    }
}

static bool wrap_rmdir(const char *path, int *error, void *context)
{
    fs_in();
    SMTPcontext_t *ctx = (SMTPcontext_t*)context;
    int ret = ((SMTPFileSystem*)ctx->fs)->rmdir(path, error, (SMTPcontext_t*)context);
    if(ret == 0){
        *error = 0;
        return true;
    }
    else {
        *error = ret;
        return false;
    }
}

static bool wrap_rename(const char *path, const char *newpath, int *error, void *context)
{
    fs_in();
    SMTPcontext_t *ctx = (SMTPcontext_t*)context;
    int ret = ((SMTPFileSystem*)ctx->fs)->rename(path, newpath, error, (SMTPcontext_t*)context);
    if(ret == 0){
        *error = 0;
        return true;
    }
    else {
        *error = ret;
        return false;
    }
}

static bool wrap_utime(const char *path, const kfstime_t *atime, const kfstime_t *mtime, int *error, void *context)
{
    fs_in();
    SMTPcontext_t *ctx = (SMTPcontext_t*)context;
    int ret = ((SMTPFileSystem*)ctx->fs)->utime(path, atime, mtime, error, (SMTPcontext_t*)context);
    if(ret == 0){
        *error = 0;
        return true;
    }
    else {
        *error = ret;
        return false;
    }
}

static ssize_t wrap_read(const char *path, char *buf, size_t offset, size_t length, int *error, void *context)
{
    fs_in();
    SMTPcontext_t *ctx = (SMTPcontext_t*)context;
    int ret = ((SMTPFileSystem*)ctx->fs)->read(path, buf, offset, length, error, (SMTPcontext_t*)context);
    if(ret == 0){
        *error = 0;
        return true;
    }
    else {
        *error = ret;
        return false;
    }
}

static ssize_t wrap_write(const char *path, const char *buf, size_t offset, size_t length, int *error, void *context)
{
    fs_in();
    SMTPcontext_t *ctx = (SMTPcontext_t*)context;
    int ret = ((SMTPFileSystem*)ctx->fs)->write(path, buf, offset, length, error, (SMTPcontext_t*)context);
    if(ret == 0){
        *error = 0;
        return true;
    }
    else {
        *error = ret;
        return false;
    }
}

static bool wrap_truncate(const char *path, uint64_t size, int *error, void *context)
{
    fs_in();
    SMTPcontext_t *ctx = (SMTPcontext_t*)context;
    int ret = ((SMTPFileSystem*)ctx->fs)->truncate(path, size);
    if(ret == 0){
        *error = 0;
        return true;
    }
    else {
        *error = ret;
        return false;
    }
}

static bool wrap_statfs(const char *path, kfsstatfs_t *result, int *error, void *context)
{
    fs_in();
    SMTPcontext_t *ctx = (SMTPcontext_t*)context;
    int ret = ((SMTPFileSystem*)ctx->fs)->statfs(path, result, error, (SMTPcontext_t*)context);
    if(ret == 0){
        *error = 0;
        return true;
    }
    else {
        *error = ret;
        return false;
    }
}

static bool wrap_readdir(const char *path, kfscontents_t *contents, int *error, void *context)
{
    fs_in();
    SMTPcontext_t *ctx = (SMTPcontext_t*)context;
    int ret = ((SMTPFileSystem*)ctx->fs)->readdir(path, contents, error, (SMTPcontext_t*)context);
    if(ret == 0){
        *error = 0;
        return true;
    }
    else {
        *error = ret;
        return false;
    }
}

static bool wrap_symlink(const char *path, const char *value, int *error, void *context){
    return false;
}

static bool wrap_readlink(const char *path, char **value, int *error, void *context) {
    return false;
}

static bool wrap_chmod(const char *path, kfsmode_t mode, int *error, void *context) {
    return false;
}

static bool wrap_create(const char *path, int *error, void *context)
{
    fs_in();
    SMTPcontext_t *ctx = (SMTPcontext_t*)context;
    int ret = ((SMTPFileSystem*)ctx->fs)->create(path, error, (SMTPcontext_t*)context);
    if(ret == 0){
        *error = 0;
        return true;
    }
    else {
        *error = ret;
        return false;
    }
}

static bool wrap_remove(const char *path, int *error, void *context){
    fs_in();
    SMTPcontext_t *ctx = (SMTPcontext_t*)context;
    int ret = ((SMTPFileSystem*)ctx->fs)->unlink(path, error, (SMTPcontext_t*)context);
    if(ret == 0){
        *error = 0;
        return true;
    }
    else {
        *error = ret;
        return false;
    }
}

// not supported in kfs
static bool wrap_fsync(const char *path, int *error, void *context){
    fs_in();
    SMTPcontext_t *ctx = (SMTPcontext_t*)context;
    int ret = ((SMTPFileSystem*)ctx->fs)->fsync(path, error, (SMTPcontext_t*)context);
    if(ret == 0){
        *error = 0;
        return true;
    }
    else {
        *error = ret;
        return false;
    }
}


// -----------------------------------------------------------------------------

SMTPFileSystem::SMTPFileSystemOptions::SMTPFileSystemOptions()
    : m_good(false)
    , m_help(false)
    , m_version(false)
    , m_verbose(false)
    , m_enable_move(false)
    , m_list_devices(false)
    , m_device_no(1)
    , m_device_file(nullptr)
    , m_mount_point(nullptr)
{
}

SMTPFileSystem::SMTPFileSystemOptions::~SMTPFileSystemOptions()
{
    free(static_cast<void*>(m_device_file));
    free(static_cast<void*>(m_mount_point));
}

// -----------------------------------------------------------------------------

SMTPFileSystem::SMTPFileSystem():
m_device(),
m_kfs_filesystem(),
m_tmp_files_pool(),
m_options()
{
    return;
}

SMTPFileSystem::~SMTPFileSystem()
{
    kfs_unmount(m_kfs_id);
    
    m_device.disconnect();

    if (!m_tmp_files_pool.removeTmpDir()) {
        logerr("Can not remove a temporary directory.\n");
        return;
    }
    return;
}

enum opt_result{
    OPT_LIST_ALL,
    OPT_MOUNT_DEV,
    OPT_BAD_ARG
};

struct mntopts {
    int device_idx;
    char* mntpt;
};

int getmntopts(int argc, char **argv, struct mntopts &mount_opts){
    int c;
    int opt_ind;
    int device_idx;
    bool good;
    
    struct option long_opts[] = {
        { "all", no_argument, 0, 0 },
        { "device", required_argument, 0, 1 },
        { 0, 0, 0, 0 }
    };
    opterr = 0;
    while ((c = getopt_long(argc, argv, "ad:", long_opts, &opt_ind)) != -1) {
        switch (c) {
        case OPT_LIST_ALL:
        case 'a':
            return OPT_LIST_ALL;
        case OPT_MOUNT_DEV:
        case 'd': {
            char *end = nullptr;
            mount_opts.device_idx = static_cast<int>(strtol(optarg, &end, 10));
            good = end[0] == '\0';
            break;
        }
        case '?':
            return OPT_BAD_ARG;;
        }
    }
    
    argc -= optind;
    argv += optind;
    
    mount_opts.mntpt = argv[0];
    
    return OPT_MOUNT_DEV;
}

bool SMTPFileSystem::parseOptions(int argc, char **argv)
{
    int ret;

    if (argc < 2) {
        logerr("Wrong usage.\n");
        m_options.m_good = false;
        return false;
    }
    struct mntopts opts;
    
    if((ret = getmntopts(argc, argv, opts)) == OPT_BAD_ARG){
        m_options.m_good = true;
        m_options.m_help = true;
        return true;
    }
    
    if (ret == OPT_LIST_ALL) {
        m_options.m_good = true;
        m_options.m_list_devices = true;
    }

    if (ret == OPT_MOUNT_DEV) {
        m_options.m_good = true;
        m_options.m_mount_point = opts.mntpt;
        m_options.m_device_no = opts.device_idx;
        m_options.m_good = true;
        m_options.m_verbose = true;
    }
    
    if (m_options.m_version || m_options.m_help || m_options.m_list_devices) {
        m_options.m_good = true;
        return true;
    }

    if (m_options.m_device_file && !m_options.m_mount_point) {
        m_options.m_mount_point = m_options.m_device_file;
        m_options.m_device_file = nullptr;
    }

    if (!m_options.m_mount_point) {
        logerr("Mount point missing.\n");
        m_options.m_good = false;
        return false;
    }

    if (m_options.m_verbose) {
        Logger::setGlobalVerbose();
    }

    --m_options.m_device_no;

    // device file and -- device are mutually exclusive, fail if both set
    if (m_options.m_device_no && m_options.m_device_file) {
        m_options.m_good = false;
        return false;
    }

    m_options.m_good = true;
    return true;
}

void SMTPFileSystem::printHelp(std::string progname) const
{
    std::cerr << "usage: " << smtpfs_basename(progname)
              << " <source> mountpoint [options]\n\n"
        << "general options:\n"
        << "    -o opt,[opt...]        mount options\n"
        << "    -h   --help            print help\n"
        << "    -V   --version         print version\n\n"
        << "simple-mtpfs options:\n"
        << "    -v   --verbose         verbose output, implies -f\n"
        << "    -l   --list-devices    print available devices. Supports <source> option\n"
        << "         --device          select a device number to mount\n"
        << "    -o enable-move         enable the move operations\n\n";
        std::cerr << "\nReport bugs to <" << PACKAGE_BUGREPORT << ">.\n";
}

void SMTPFileSystem::printVersion() const
{
    std::cout << "simple-mtpfs version " << 1.0 << "\n";
}

bool SMTPFileSystem::listDevices() const
{
    const std::string dev_file = m_options.m_device_file ? m_options.m_device_file : "";
    return MTPDevice::listDevices(m_options.m_verbose, dev_file);
}

bool SMTPFileSystem::mount(SMTPcontext_t *ctx){

    if (m_options.m_version || m_options.m_help)
        return false;

    if (!smtpfs_check_dir(m_options.m_mount_point)) {
        logerr("Can not mount the device to '", m_options.m_mount_point, "'.\n");
        return false;
    }

    if (!m_tmp_files_pool.createTmpDir()) {
        logerr("Can not create a temporary directory.\n");
        return false;
    }

    if (m_options.m_device_file) {
        // Try to use device file first, if provided
        if (!m_device.connect(m_options.m_device_file))
            return false;
    } else {
        // Connect to MTP device by order number, if no device file supplied
        if (!m_device.connect(m_options.m_device_no))
            return false;
    }
        
    kfsoptions_t opts = {m_options.m_mount_point};
    
    m_kfs_filesystem.options = opts;
    m_kfs_filesystem.context = ctx;
    m_kfs_filesystem.statfs = wrap_statfs;
    m_kfs_filesystem.stat = wrap_getattr;
    m_kfs_filesystem.read = wrap_read;
    m_kfs_filesystem.write = wrap_write;
    m_kfs_filesystem.symlink = wrap_symlink;    // 0
    m_kfs_filesystem.readlink = wrap_readlink;  // 0
    m_kfs_filesystem.create = wrap_create;
    m_kfs_filesystem.remove = wrap_remove;
    m_kfs_filesystem.rename = wrap_rename;
    m_kfs_filesystem.chmod = wrap_chmod;        // 0
    m_kfs_filesystem.utimes = wrap_utime;
    m_kfs_filesystem.mkdir = wrap_mkdir;
    m_kfs_filesystem.rmdir = wrap_rmdir;
    m_kfs_filesystem.readdir = wrap_readdir;
    m_kfs_filesystem.truncate = wrap_truncate;
    
    m_kfs_id = kfs_mount(&m_kfs_filesystem);
    if (m_kfs_id < 0) { kfs_perror("mount"); return false; }
    return true;
}

int SMTPFileSystem::getattr(const char *path, kfsstat_t *result, int *error, SMTPcontext_t *context)
{
    int ret = 0;
    memset(result, 0, sizeof(struct kfsstat));
    
    if (std::string(path) == std::string("/")) {
        const TypeDir *content = opendir(path);
        if (content == NULL){
            ret = 0;
            goto out;
        }
        
        result->size = content->dirCount();
        result->mode = (kfsmode_t)(0775);
        result->type = KFS_DIR;
        ret = 0;
        goto out;
    }
    else {
        struct stat sbuf = {0};
        std::string tmp_path(smtpfs_dirname(path));
        std::string tmp_file(smtpfs_basename(path));
        const TypeDir *content = m_device.dirFetchContent(tmp_path);
        
        if (!content) {
            ret = ENOENT;
            goto out;
        }
        
        if (content->dir(tmp_file)) {
            const TypeDir *dir = content->dir(tmp_file);
            stat(tmp_file.c_str(), &sbuf);
            result->type = KFS_DIR;
            result->mode = (kfsmode_t)(S_IFDIR | 0775);
            result->mtime.nsec = dir->modificationDate();
        }
        else if (content->file(tmp_file)) {
            const TypeFile *file = content->file(tmp_file);
            stat(tmp_file.c_str(), &sbuf);
            result->size = file->size();
            result->mode = static_cast<kfsmode_t>(result->mode | 0644);
            result->type = KFS_REG;
            result->mtime.nsec = file->modificationDate();
            result->atime = result->mtime;
            result->ctime = result->mtime;
            result->used = (file->size() / 512) + (file->size() % 512 > 0 ? 1 : 0);
        }
        else {
            ret = ENOENT;
            goto out;
        }
    }
out:
    fs_out();
    return ret;
}


int SMTPFileSystem::mkdir(const char *path, int *error, SMTPcontext_t *context)
{
    int ret = m_device.dirCreateNew(std::string(path));
    fs_out();
    return ret;
}

int SMTPFileSystem::unlink(const char *path, int *error, SMTPcontext_t *context)
{
    int ret = m_device.fileRemove(std::string(path));
    fs_out();
    return ret;
}

int SMTPFileSystem::rmdir(const char *path, int *error, SMTPcontext_t *context)
{
    int ret = m_device.dirRemove(std::string(path));
    fs_out();
    return ret;
}

int SMTPFileSystem::rename(const char *path, const char *newpath, int *error, SMTPcontext_t *context)
{
    int ret = 0;
    const std::string tmp_old_dirname(smtpfs_dirname(std::string(path)));
    const std::string tmp_new_dirname(smtpfs_dirname(std::string(newpath)));
    std::string tmp_file;
    if (tmp_old_dirname == tmp_new_dirname){
        ret = m_device.rename(std::string(path), std::string(newpath));
        goto out;
    }

    if (!m_options.m_enable_move){
        ret = EPERM;
        goto out;
    }

    tmp_file = (m_tmp_files_pool.makeTmpPath(std::string(newpath)));
    ret = m_device.filePull(std::string(path), tmp_file);
    if (ret != 0)
        goto out;

    ret = m_device.filePush(tmp_file, std::string(newpath));
    if (ret != 0)
        goto out;

    ret = m_device.fileRemove(std::string(path));
    if (ret != 0)
        goto out;
out:
    fs_out();
    return ret;
}

int SMTPFileSystem::utime(const char *path, const kfstime_t *atime, const kfstime_t *mtime, int *error, SMTPcontext_t *context)
{
    std::string tmp_basename(smtpfs_basename(std::string(path)));
    std::string tmp_dirname(smtpfs_dirname(std::string(path)));

    const TypeDir *parent = m_device.dirFetchContent(tmp_dirname);
    if (!parent){
        fs_out();
        return -ENOENT;
    }

    const TypeFile *file = parent->file(tmp_basename);
    if (!file){
        fs_out();
        return -ENOENT;
    }

//    const_cast<TypeFile*>(file)->setModificationDate((time_t)mtime->nsec);
    fs_out();
    return 0;
}

int SMTPFileSystem::create(const char *path, int *error, SMTPcontext_t *context)
{
    const std::string tmp_path = m_tmp_files_pool.makeTmpPath(std::string(path));

    int rval = ::creat(tmp_path.c_str(), O_CREAT | O_TRUNC | O_WRONLY);
    if (rval < 0){
        fs_out();
        return -errno;
    }

    m_tmp_files_pool.addFile(TypeTmpFile(std::string(path), tmp_path, rval, true));
    rval = m_device.filePush(tmp_path, std::string(path));

    if (rval != 0){
        fs_out();
        return rval;
    }
    fs_out();
    return 0;
}

int SMTPFileSystem::fsync(const char *path, int *error, SMTPcontext_t *context)
{
    int rval;
    
    const std::string std_path(path);
    if (std_path == std::string("-")){
        fs_out();
        return 0;
    }

    TypeTmpFile *tmp_file = const_cast<TypeTmpFile*>(
        m_tmp_files_pool.getFile(std_path));
    
    if (tmp_file->refcnt() != 0){
        fs_out();
        return 0;
    }
    
    tmp_file->close();

    const bool modif = tmp_file->isModified();
    const std::string tmp_path = tmp_file->pathTmp();
    m_tmp_files_pool.removeFile(std_path);
    if (modif) {
        rval = m_device.filePush(tmp_path, std_path);
        if (rval != 0) {
            ::unlink(tmp_path.c_str());
            fs_out();
            return -rval;
        }
    }

    ::unlink(tmp_path.c_str());
    fs_out();
    return 0;
}

int SMTPFileSystem::open(const char *path, int flags)
{
    if (flags & O_WRONLY)
        flags |= O_TRUNC;

    const std::string std_path(path);

    TypeTmpFile *tmp_file = const_cast<TypeTmpFile*>(
        m_tmp_files_pool.getFile(std_path));

    std::string tmp_path;
    if (tmp_file) {
        tmp_path = tmp_file->pathTmp();
    } else {
        tmp_path = m_tmp_files_pool.makeTmpPath(std_path);

        // only copy the file if needed
        if (!hasPartialObjectSupport()) {
            int rval = m_device.filePull(std_path, tmp_path);
            if (rval != 0)
                return -rval;
        } else {
            int fd = ::creat(tmp_path.c_str(), S_IRUSR | S_IWUSR);
            ::close(fd);
        }
    }

    // we create the tmp file even if we can use partial get/send to
    // have a valid file descriptor
    int fd = ::open(tmp_path.c_str(), flags);
    if (fd < 0) {
        ::unlink(tmp_path.c_str());
        fs_out();
        return -errno;
    }

    if (tmp_file)
        tmp_file->addFileDescriptor(fd);
    else
        m_tmp_files_pool.addFile(TypeTmpFile(std_path, tmp_path, fd));
    fs_out();
    return 0;
}

int SMTPFileSystem::read(const char *path, char *buf, size_t offset, size_t length, int *error, SMTPcontext_t *context)
{
    ssize_t rval = 0;
    if (hasPartialObjectSupport()) {
        const std::string std_path(path);
        rval = m_device.fileRead(std_path, buf, length, offset);
    }
    else {
        const TypeTmpFile *tmp_file = m_tmp_files_pool.getFile(std::string(path));
        int fd = open(tmp_file->pathTmp().c_str(), O_RDONLY);
        rval = ::pread(fd, buf, length, offset);
        if (rval < 0){
            fs_out();
            return -errno;
        }
    }
    
    fs_out();
    return rval;
}

int SMTPFileSystem::write(const char *path, const char *buf, size_t offset, size_t length, int *error, SMTPcontext_t *context)
{
    ssize_t rval = 0;
    if (hasPartialObjectSupport()) {
        const std::string std_path(path);
        rval = m_device.fileWrite(std_path, buf, length, offset);
    } else {
        const TypeTmpFile *tmp_file = m_tmp_files_pool.getFile(std::string(path));
        if (!tmp_file){
            fs_out();
            return -EINVAL;
        }
        int fd = open(tmp_file->pathTmp().c_str(), O_WRONLY);
        rval = ::pwrite(fd, buf, length, offset);
        if (rval < 0){
            fs_out();
            return -errno;
        }

        const_cast<TypeTmpFile*>(tmp_file)->setModified();
    }
    
    fs_out();
    return ((int) rval);
}

int SMTPFileSystem::statfs(const char *path, kfsstatfs_t *result, int *error, SMTPcontext_t *context)
{
    uint64_t bs = 1024;
    // XXX: linux coreutils still use bsize member to calculate free space
    result->size = m_device.storageTotalSize() / bs;
    result->free = m_device.storageFreeSize() / bs;
    fs_out();
    return 0;
}

const TypeDir* SMTPFileSystem::opendir(const char *path)
{
    const TypeDir *content = m_device.dirFetchContent(std::string(path));
    if (!content){
        return NULL;
    }
    
    return content;
}

int SMTPFileSystem::readdir(const char *path, kfscontents_t *contents, int *error, SMTPcontext_t *context)
{
    const TypeDir *content = this->opendir(path);
    if (content == NULL){
        fs_out();
        return -ENOENT;
    }
    
    // append these or else infinite loop!!!!
    kfscontents_append(contents, ".");
    kfscontents_append(contents, "..");
    
    const std::set<TypeDir> dirs = content->dirs();
    const std::set<TypeFile> files = content->files();

    for (const TypeDir &d : dirs) {
        kfscontents_append(contents, d.name().c_str());
    }

    for (const TypeFile &f : files) {
        kfscontents_append(contents, f.name().c_str());
    }
    fs_out();
    return 0;
}

int SMTPFileSystem::truncate(const char *path, off_t new_size)
{
    const std::string tmp_path = m_tmp_files_pool.makeTmpPath(std::string(path));
    int rval = m_device.filePull(std::string(path), tmp_path);
    if (rval != 0) {
        ::unlink(tmp_path.c_str());
        fs_out();
        return -rval;
    }

    rval = ::truncate(tmp_path.c_str(), new_size);
    if (rval != 0) {
        int errno_tmp = errno;
        ::unlink(tmp_path.c_str());
        fs_out();
        return -errno_tmp;
    }

    rval = m_device.fileRemove(std::string(path));
    if (rval != 0) {
        ::unlink(tmp_path.c_str());
        fs_out();
        return -rval;
    }

    rval = m_device.filePush(tmp_path, std::string(path));
    ::unlink(tmp_path.c_str());

    if (rval != 0){
        fs_out();
        return -rval;
    }

    fs_out();
    return 0;
}

bool SMTPFileSystem::hasPartialObjectSupport()
{
    MTPDevice::Capabilities caps = m_device.getCapabilities();
    return (caps.canGetPartialObject() && caps.canSendPartialObject());
}
