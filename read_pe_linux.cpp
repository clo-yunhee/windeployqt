#include <parse.h>
#include <nt-headers.h>
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

using namespace std;
using namespace peparse;

#define IMAGE_FIRST_SECTION(ntheader) ((image_section_header *) ((uintptr_t) (ntheader) + offsetof(nt_header_32, OptionalHeader) + ((ntheader))->FileHeader.SizeOfOptionalHeader))

/*
const image_section_header *findSectionHeader(std::uint32_t rva, const nt_header_32 *ntHeader)
{
    const image_section_header *section = IMAGE_FIRST_SECTION(ntHeader);
    const image_section_header *sectionEnd = section + ntHeader->FileHeader.NumberOfSections;
    for ( ; section < sectionEnd; ++section)
        if (rva >= section->VirtualAddress && rva < (section->VirtualAddress + section->Misc.VirtualSize))
            return section;
    return 0;
}

inline const void *rvaToPtr(std::uint32_t rva, const nt_header_32 *ntHeader, const void *imageBase)
{
}

inline unsigned ntHeaderWordSize(const nt_header_32 *header)
{
    if (header->OptionalMagic == NT_OPTIONAL_32_MAGIC) {
        return 32;
    }
    else if (header->OptionalMagic == NT_OPTIONAL_64_MAGIC) {
        return 64;
    }
    else {
        return 0;
    }
}

// Check for MSCV runtime (MSVCP90D.dll/MSVCP90.dll, MSVCP120D.dll/MSVCP120.dll,
// VCRUNTIME140D.DLL/VCRUNTIME140.DLL (VS2015) or msvcp120d_app.dll/msvcp120_app.dll).
enum MsvcDebugRuntimeResult { MsvcDebugRuntime, MsvcReleaseRuntime, NoMsvcRuntime };

static inline MsvcDebugRuntimeResult checkMsvcDebugRuntime(const QStringList &dependentLibraries)
{
    for (const QString &lib : dependentLibraries) {
        int pos = 0;
        if (lib.startsWith(QLatin1String("MSVCR"), Qt::CaseInsensitive)
            || lib.startsWith(QLatin1String("MSVCP"), Qt::CaseInsensitive)) {
            pos = 5;
        } else if (lib.startsWith(QLatin1String("VCRUNTIME"), Qt::CaseInsensitive)) {
            pos = 9;
        }
        if (pos && lib.at(pos).isDigit()) {
            for (++pos; lib.at(pos).isDigit(); ++pos)
                ;
            return lib.at(pos).toLower() == QLatin1Char('d')
                ? MsvcDebugRuntime : MsvcReleaseRuntime;
        }
    }
    return NoMsvcRuntime;
}

template<typename T, T nt_header_32::*mf>
inline QStringList readImportSections(const nt_header_32 *nth, const void *base, QString *errorMessage)
{
    auto& OptionalHeader = nth->*mf;
    const std::uint32_t importsStartRVA = OptionalHeader.DataDirectory[DIR_IMPORT].VirtualAddress;
    if (!importsStartRVA) {
        *errorMessage = QString::fromLatin1("Failed to find DIR_IMPORT entry.");
        return QStringList();
    }
    const import_dir_entry *importDesc = static_cast<const import_dir_entry *>(rvaToPtr(importsStartRVA, ntHeaders, base));
}

template<typename T, T nt_header_32::*mf>
inline void determineDebugAndDependentLibs(const nt_header_32 *nth, const void *fileMemory,
                                           bool isMinGW,
                                           QStringList *dependentLibrariesIn,
                                           bool *isDebugIn, QString *errorMessage)
{
    const bool hasDebugEntry = (nth->*mf).DataDirectory[DIR_DEBUG].Size;
    QStringList dependentLibraries;
    if (dependentLibrariesIn || (isDebugIn != nullptr && hasDebugEntry && !isMinGW))
        dependentLibraries = readImportSections(nth, fileMemory, errorMessage);


    if (dependentLibrariesIn)
        *dependentLibrariesIn = dependentLibraries;
    if (isDebugIn != nullptr) {
        if (isMinGW) {
            // Use logic that's used e.g. in objdump / pfd library
            *isDebugIn = !(nth->FileHeader.Characteristics & IMAGE_FILE_DEBUG_STRIPPED);
        } else {
            // When an MSVC debug entry is present, check whether the debug runtime
            // is actually used to detect -release / -force-debug-info builds.
            *isDebugIn = hasDebugEntry && checkMsvcDebugRuntime(dependentLibraries) != MsvcReleaseRuntime;
        }
    }
}*/

bool readPeExecutable(const QString &peExecutableFileName, QString *errorMessage,
                      QStringList *dependentLibrariesIn, unsigned *wordSizeIn,
                      bool *isDebugIn, bool isMinGW, unsigned short *machineArchIn)
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

    IterImpVAString(p, [](void *cbd, const VA &addr, const std::string &moduleName, const std::string &symbolName) -> int {
        auto ctx = static_cast<context *>(cbd);
        ctx->moduleNames.insert(QString::fromStdString(moduleName));
        return 0;
    }, &ctx);

    if (dependentLibrariesIn)
        *dependentLibrariesIn = ctx.moduleNames.toList();

    if (isDebugIn)
        *isDebugIn = false;

    if (machineArchIn)
        *machineArchIn = p->peHeader.nt.FileHeader.Machine;

    DestructParsedPE(p);
    
    return true;
}

QString findD3dCompiler(Platform, const QString &, unsigned)
{
    return QString();
}


