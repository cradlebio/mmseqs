#include "Util.h"
#include "Parameters.h"
#include "Debug.h"
#include "FileUtil.h"
#include "CommandCaller.h"

#include <iomanip>
#include <cassert>

#include "databases.sh.h"

struct EnvironmentEntry {
    const char* key;
    const char* value;
};

struct DatabaseDownload {
    const char *name;
    const char *description;
    const char *citation;
    const char *url;
    bool hasTaxonomy;
    int dbType;
    const unsigned char *script;
    size_t scriptLength;
    std::vector<EnvironmentEntry> environment;
};

std::vector<DatabaseDownload> downloads = {{
   "UniRef100",
   "The UniProt Reference Clusters provide clustered sets of sequences from the UniProt Knowledgebase.",
   "Suzek et al: UniRef: comprehensive and non-redundant UniProt reference clusters. Bioinformatics 23(10), 1282–1288 (2007)",
   "https://www.uniprot.org/help/uniref",
   true, Parameters::DBTYPE_AMINO_ACIDS, databases_sh, databases_sh_len,
   { }
}, {
    "UniRef90",
    "The UniProt Reference Clusters provide clustered sets of sequences from the UniProt Knowledgebase.",
    "Suzek et al: UniRef: comprehensive and non-redundant UniProt reference clusters. Bioinformatics 23(10), 1282–1288 (2007)",
    "https://www.uniprot.org/help/uniref",
    true, Parameters::DBTYPE_AMINO_ACIDS, databases_sh, databases_sh_len,
    { }
}, {
    "UniRef50",
    "The UniProt Reference Clusters provide clustered sets of sequences from the UniProt Knowledgebase.",
    "Suzek et al: UniRef: comprehensive and non-redundant UniProt reference clusters. Bioinformatics 23(10), 1282–1288 (2007)",
    "https://www.uniprot.org/help/uniref",
    true, Parameters::DBTYPE_AMINO_ACIDS, databases_sh, databases_sh_len,
    { }
}, {
    "UniProtKB",
    "The UniProt Knowledgebase is the central hub for the collection of functional information on proteins, with accurate, consistent and rich annotation.",
    "The UniProt Consortium: UniProt: a worldwide hub of protein knowledge. Nucleic Acids Res 47(D1), D506-515 (2019)",
    "https://www.uniprot.org/help/uniprotkb",
    true, Parameters::DBTYPE_AMINO_ACIDS, databases_sh, databases_sh_len,
    { }
}, {
    "UniProtKB/TrEMBL",
    "UniProtKB/TrEMBL (unreviewed) contains protein sequences associated with computationally generated annotation and large-scale functional characterization.",
    "The UniProt Consortium: UniProt: a worldwide hub of protein knowledge. Nucleic Acids Res 47(D1), D506-515 (2019)",
    "https://www.uniprot.org/help/uniprotkb",
    true, Parameters::DBTYPE_AMINO_ACIDS, databases_sh, databases_sh_len,
    { }
}, {
    "UniProtKB/Swiss-Prot",
    "UniProtKB/Swiss-Prot (reviewed) is a high quality manually annotated and non-redundant protein sequence database, which brings together experimental results, computed features and scientific conclusions.",
    "The UniProt Consortium: UniProt: a worldwide hub of protein knowledge. Nucleic Acids Res 47(D1), D506-515 (2019)",
    "https://uniprot.org",
    true, Parameters::DBTYPE_AMINO_ACIDS, databases_sh, databases_sh_len,
    { }
}, {
    "NR",
    "Non-redundant protein sequences from GenPept, Swissprot, PIR, PDF, PDB, and NCBI RefSeq.",
    "NCBI Resource Coordinators: Database resources of the National Center for Biotechnology Information. Nucleic Acids Res 46(D1), D8-D13 (2018)",
    "https://ftp.ncbi.nlm.nih.gov/blast/db/FASTA",
    false, Parameters::DBTYPE_AMINO_ACIDS, databases_sh, databases_sh_len,
    { }
}, {
    "NT",
    "Partially non-redundant nucleotide sequences from all traditional divisions of GenBank, EMBL, and DDBJ excluding GSS, STS, PAT, EST, HTG, and WGS.",
    "NCBI Resource Coordinators: Database resources of the National Center for Biotechnology Information. Nucleic Acids Res 46(D1), D8-D13 (2018)",
    "https://ftp.ncbi.nlm.nih.gov/blast/db/FASTA",
    false, Parameters::DBTYPE_NUCLEOTIDES, databases_sh, databases_sh_len,
    { }
}, {
    "PDB",
    "The Protein Data Bank is the single worldwide archive of structural data of biological macromolecules.",
    "Berman et al: The Protein Data Bank. Nucleic Acids Res 28(1), 235-242 (2000)",
    "https://www.rcsb.org",
    false, Parameters::DBTYPE_AMINO_ACIDS, databases_sh, databases_sh_len,
    { }
}, {
    "PDB70",
    "PDB clustered to 70% sequence identity and enriched using HHblits with Uniclust sequences.",
    "Steinegger et al: HH-suite3 for fast remote homology detection and deep protein annotation. BMC Bioinform 20(1), 473 (2019)",
    "https://github.com/soedinglab/hh-suite",
    false, Parameters::DBTYPE_HMM_PROFILE, databases_sh, databases_sh_len,
    { }
}, {
    "Pfam-A.full",
    "The Pfam database is a large collection of protein families, each represented by multiple sequence alignments and hidden Markov models.",
    "El-Gebali and Mistry et al: The Pfam protein families database in 2019. Nucleic Acids Res 47(D1), D427-D432 (2019)",
    "https://pfam.xfam.org",
    false, Parameters::DBTYPE_HMM_PROFILE, databases_sh, databases_sh_len,
    { }
}, {
    "Pfam-A.seed",
    "The Pfam database is a large collection of protein families, each represented by multiple sequence alignments and hidden Markov models.",
    "El-Gebali and Mistry et al: The Pfam protein families database in 2019. Nucleic Acids Res 47(D1), D427-D432 (2019)",
    "https://pfam.xfam.org",
    false, Parameters::DBTYPE_HMM_PROFILE, databases_sh, databases_sh_len,
    { }
}};


std::string listDatabases(const Command &command, bool detailed) {
    size_t nameWidth = 0, urlWidth = 0, dbTypeWidth = 0;
    for (size_t i = 0; i < downloads.size(); ++i) {
        nameWidth = std::max(nameWidth, strlen(downloads[i].name));
        urlWidth = std::max(urlWidth, strlen(downloads[i].url));
        dbTypeWidth = std::max(dbTypeWidth, strlen(Parameters::getDbTypeName(downloads[i].dbType)));
    }

    std::stringstream description;
    if (detailed && strlen(command.longDescription) != 0) {
        description << command.longDescription;
    } else {
        description << command.shortDescription;
    }
    if (detailed) {
        description << "\n By " << command.author;
    }

    description << std::boolalpha << std::left;
    description << "\n\n  " << std::setw(nameWidth) << "Database" << "\t" <<  std::setw(dbTypeWidth) << "Type" << "\t" << std::setw(8) << "Taxonomy" << "\t" << std::setw(urlWidth) << "Url" << "\n";

    for (size_t i = 0; i < downloads.size(); ++i) {
        description << "- "
                    << std::setw(nameWidth) << downloads[i].name << "\t"
                    << std::setw(dbTypeWidth) << Parameters::getDbTypeName(downloads[i].dbType) << "\t"
                    << std::right << std::setw(8) << (downloads[i].hasTaxonomy ? "yes" : "-") << "\t"
                    << std::left  << std::setw(urlWidth) << downloads[i].url << "\n";
        if (detailed) {
            if (strlen(downloads[i].description) > 0) {
                description << "  " << downloads[i].description << "\n";
            }
            if (strlen(downloads[i].citation) > 0) {
                description << "  Cite: " << downloads[i].citation << "\n";
            }
        }
    }

    return description.str();
}

int databases(int argc, const char **argv, const Command &command) {
    Parameters &par = Parameters::getInstance();
    par.parseParameters(argc, argv, command, false, Parameters::PARSE_ALLOW_EMPTY, 0);

    std::string description = listDatabases(command, par.help);
    Command copy = command;
    copy.longDescription = description.c_str();
    copy.shortDescription = description.c_str();
    if (par.filenames.size() == 0) {
        par.printUsageMessage(copy, par.help ? MMseqsParameter::COMMAND_EXPERT : 0);
        EXIT(EXIT_SUCCESS);
    }

    ssize_t downloadIdx = -1;
    for (size_t i = 0; i < downloads.size(); ++i) {
        if (par.db1 == std::string(downloads[i].name)) {
            downloadIdx = i;
            break;
        }
    }
    if (downloadIdx == -1) {
        par.printUsageMessage(copy, par.help ? MMseqsParameter::COMMAND_EXPERT : 0);
        Debug(Debug::ERROR) << "Selected database " << par.db1 << " was not found\n";
        EXIT(EXIT_FAILURE);
    }

    std::string tmpDir = par.db3;
    std::string hash = SSTR(par.hashParameter(par.filenames, par.databases));
    if (par.reuseLatest) {
        hash = FileUtil::getHashFromSymLink(tmpDir + "/latest");
    }
    tmpDir = FileUtil::createTemporaryDirectory(tmpDir, hash);
    par.filenames.pop_back();
    par.filenames.push_back(tmpDir);

    CommandCaller cmd;
    for (size_t i = 0; i < downloads[downloadIdx].environment.size(); ++i) {
        cmd.addVariable(downloads[downloadIdx].environment[i].key, downloads[downloadIdx].environment[i].value);
    }
    cmd.addVariable("TAXONOMY", downloads[downloadIdx].hasTaxonomy ? "TRUE" : NULL);
    cmd.addVariable("REMOVE_TMP", par.removeTmpFiles ? "TRUE" : NULL);
    cmd.addVariable("VERB_PAR", par.createParameterString(par.onlyverbosity).c_str());
    cmd.addVariable("COMP_PAR", par.createParameterString(par.verbandcompression).c_str());
    cmd.addVariable("THREAD_NUM", SSTR(par.threads).c_str());
    cmd.addVariable("THREADS_PAR", par.createParameterString(par.onlythreads).c_str());
    cmd.addVariable("THREADS_COMP_PAR", par.createParameterString(par.threadsandcompression).c_str());
    std::string program = tmpDir + "/download.sh";
    FileUtil::writeFile(program, downloads[downloadIdx].script, downloads[downloadIdx].scriptLength);
    cmd.execProgram(program.c_str(), par.filenames);

    // Should never get here
    assert(false);
    EXIT(EXIT_FAILURE);
}