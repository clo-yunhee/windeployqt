#include <pe-parse/parse.h>
#include <pe-parse/nt-headers.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <deque>
#include <string>
#include <list>
#include <utility>
#include <filesystem>
#include <stdexcept>
#include <QSet>
#include <QList>
#include <QString>

using namespace std;
using namespace peparse;

#define IMAGE_FIRST_SECTION(ntheader) ((image_section_header *) ((uintptr_t) (ntheader) + offsetof(nt_header_32, OptionalHeader) + ((ntheader))->FileHeader.SizeOfOptionalHeader))
 
bool readPeExecutable(const QString &peExecutableFileName, QString *,
                      QStringList *dependentLibrariesIn, unsigned *wordSize,
                      bool *isDebugIn, bool, unsigned short *machineArchIn)
{
    // Suppress stderr for the duration of the ParsePEFromFile function call.
    int fderr = dup(STDERR_FILENO);
    int fd = open("/dev/null", O_WRONLY);
    dup2(fd, STDERR_FILENO);
    close(fd);

    parsed_pe *p = ParsePEFromFile(peExecutableFileName.toLatin1());

    // Restore stderr.
    dup2(fderr, STDERR_FILENO);
    close(fderr);

    if (p == nullptr) {
        return false;
    }

    struct context {
        QSet<QString> moduleNames;
    } ctx;

    IterImpVAString(p, [](void *cbd, const VA &, const std::string &moduleName, const std::string &) -> int {
        auto ctx = static_cast<context *>(cbd);
        ctx->moduleNames.insert(QString::fromStdString(moduleName));
        return 0;
    }, &ctx);

    if (dependentLibrariesIn)
        *dependentLibrariesIn =
            QStringList(ctx.moduleNames.begin(), ctx.moduleNames.end());

    if (isDebugIn)
        *isDebugIn = false;

    if (wordSize)
        *wordSize = 64;

    if (machineArchIn)
        *machineArchIn = p->peHeader.nt.FileHeader.Machine;

    DestructParsedPE(p);
    
    return true;
}

QString findD3dCompiler(Platform, const QString &, unsigned)
{
    return QString();
}


