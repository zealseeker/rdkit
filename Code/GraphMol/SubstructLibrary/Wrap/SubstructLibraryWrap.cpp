//  Copyright (c) 2017-2019, Novartis Institutes for BioMedical Research Inc.
//  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Novartis Institutes for BioMedical Research Inc.
//       nor the names of its contributors may be used to endorse or promote
//       products derived from this software without specific prior written
//       permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#include <RDBoost/python.h>
#include <RDBoost/Wrap.h>
#include <GraphMol/RDKitBase.h>
#include <RDBoost/python_streambuf.h>

#include <GraphMol/SubstructLibrary/SubstructLibrary.h>
#include <GraphMol/SubstructLibrary/PatternFactory.h>

namespace python = boost::python;
using boost_adaptbx::python::streambuf;

namespace RDKit {

const char *MolHolderBaseDoc =
    "Base class for holding molecules used in the Substructure Library.\n"
    "Instantiations of this class are passed into the SubstructureLibrary.\n"
    "The API is quite simple: \n"
    "  AddMol(mol) -> adds a molecule to the molecule holder, returns index of "
    "molecule\n"
    "  GetMol(idx) -> return the molecule at index idx\n";

const char *MolHolderDoc =
    "Holds raw in-memory molecules\n"
    "  AddMol(mol) -> adds a molecule to the molecule holder, returns index of "
    "molecule\n"
    "  GetMol(idx,sanitize=True) -> return the molecule at index idx\n";

const char *CachedMolHolderDoc =
    "Holds molecules in their binary representation.\n"
    "This allows more molecules to be held in memory at a time\n"
    "  AddMol(mol) -> adds a molecule to the molecule holder, returns index of "
    "molecule\n\n"
    "  AddBinary(data) -> adds a picked molecule molecule to the molecule "
    "holder, returns index of molecule\n"
    "                     The data is stored as-is, no checking is done for "
    "validity.\n"
    "  GetMol(idx) -> return the molecule at index idx\n";

const char *CachedSmilesMolHolderDoc =
    "Holds molecules as smiles string\n"
    "This allows more molecules to be held in memory at a time\n"
    "  AddMol(mol) -> adds a molecule to the molecule holder, returns index of "
    "molecule\n\n"
    "  AddSmiles(smiles) -> adds a smiles string to the molecule holder, "
    "returns index of molecule\n"
    "                       The smiles is stored as-is, no checking is done "
    "for validity.\n"
    "  GetMol(idx) -> return the molecule at index idx\n";

const char *CachedTrustedSmilesMolHolderDoc =
    "Holds molecules as trusted smiles string\n"
    "This allows more molecules to be held in memory at a time and avoids "
    "RDKit sanitization\n"
    "overhead.\n"
    "See: "
    "http://rdkit.blogspot.com/2016/09/avoiding-unnecessary-work-and.html\n"
    "  AddMol(mol) -> adds a molecule to the molecule holder, returns index of "
    "molecule\n\n"
    "  AddSmiles(smiles) -> adds a smiles string to the molecule holder, "
    "returns index of molecule\n"
    "                       The smiles is stored as-is, no checking is done "
    "for validity.\n"
    "  GetMol(idx,s) -> return the molecule at index idx, \n"
    "              note, only light sanitization is done here, for instance\n"
    "              the molecules RingInfo is not initialized\n";

const char *PatternHolderDoc =
    "Holds fingerprints with optional, user-defined number of bits (default: "
    "2048) used for filtering of molecules.";
const char *TautomerPatternHolderDoc =
    "Holds tautomeric fingerprints with optional, user-defined number of bits "
    "(default: "
    "2048) used for filtering of molecules.\n"
    "These fingerprints are designed to be used with TautomerQueries.";

const char *SubstructLibraryDoc =
    "SubstructLibrary: This provides a simple API for substructure searching "
    "large datasets\n"
    "The SubstructLibrary takes full advantage of available threads during the "
    "search operation.\n"
    "Basic operation is simple\n"
    "\n"
    ">>> from __future__ import print_function\n"
    ">>> import os\n"
    ">>> from rdkit import Chem, RDConfig\n"
    ">>> from rdkit.Chem import rdSubstructLibrary\n"
    ">>> library = rdSubstructLibrary.SubstructLibrary()\n"
    ">>> for mol in Chem.SDMolSupplier(os.path.join(RDConfig.RDDataDir, \n"
    "...                               'NCI', 'first_200.props.sdf')):\n"
    "...   idx = library.AddMol(mol)\n"
    ">>> core = Chem.MolFromSmarts('CCCCOC')\n"
    ">>> indices = library.GetMatches(core)\n"
    ">>> len(indices)\n"
    "11\n"
    "\n"
    "Substructure matching options can be sent into GetMatches:\n"
    "\n"
    ">>> indices = library.GetMatches(core, useChirality=False) \n"
    ">>> len(indices)\n"
    "11\n"
    "\n"
    "Controlling the number of threads or the maximum number of matches "
    "returned:\n"
    "is also available (the default is to run on all cores)\n"
    "\n"
    ">>> indices = library.GetMatches(core, numThreads=2, maxResults=10) \n"
    ">>> len(indices)\n"
    "10\n"
    "\n"
    "Working on larger datasets:\n"
    "\n"
    "Molecules are fairly large objects and will limit the number that can be "
    "kept in memory.\n"
    "To assist this we supply three other molecule holders:\n"
    "  CachedMolHolder - stores molecules as their pickled representation\n"
    "\n"
    "  CachedSmilesMolHolder - stores molecules internally as smiles strings\n"
    "\n"
    "  CachedTrustedSmilesMolHolder = excepts (and stores) molecules as "
    "trusted smiles strings\n"
    "\n"
    "Using Pattern fingerprints as a pre-filter:"
    "\n"
    "Pattern fingerprints provide an easy way to indicate whether the "
    "substructure search should be\n"
    "be done at all.  This is particularly useful with the Binary and Smiles "
    "based molecule holders\n"
    "as they have an expensive molecule creation step in addition to the "
    "substructure searching step\n "
    "\n"
    ">>> library = "
    "rdSubstructLibrary.SubstructLibrary(rdSubstructLibrary."
    "CachedSmilesMolHolder(), \n"
    "...                                               "
    "rdSubstructLibrary.PatternHolder())\n"
    ">>> for mol in Chem.SDMolSupplier(os.path.join(RDConfig.RDDataDir, \n"
    "...                               'NCI', 'first_200.props.sdf')):\n"
    "...   idx = library.AddMol(mol)\n"
    ">>> indices = library.GetMatches(core)\n"
    ">>> len(indices)\n"
    "11\n"
    "\n"
    "This (obviously) takes longer to initialize.  However, both the molecule "
    "and pattern\n"
    "holders can be populated with raw data, a simple example is below:\n"
    "\n"
    ">>> import csv\n"
    ">>> molholder = rdSubstructLibrary.CachedSmilesMolHolder()\n"
    ">>> pattern_holder = rdSubstructLibrary.PatternHolder()\n"
    ">>> for i, row in "
    "enumerate(csv.reader(open(os.path.join(RDConfig.RDDataDir, \n"
    "...                               'NCI', 'first_200.tpsa.csv')))):\n"
    "...   if i:\n"
    "...     idx = molholder.AddSmiles(row[0])\n"
    "...     idx2 = pattern_holder.AddFingerprint(\n"
    "...         pattern_holder.MakeFingerprint(Chem.MolFromSmiles(row[0])))\n"
    "...     assert idx==idx2\n"
    ">>> library = "
    "rdSubstructLibrary.SubstructLibrary(molholder,pattern_holder)\n"
    ">>> indices = library.GetMatches(core)\n"
    ">>> len(indices)\n"
    "11\n"
    "";

python::object SubstructLibrary_Serialize(const SubstructLibrary &cat) {
  std::string res = cat.Serialize();
  python::object retval = python::object(
      python::handle<>(PyBytes_FromStringAndSize(res.c_str(), res.length())));
  return retval;
}

struct substructlibrary_pickle_suite : python::pickle_suite {
  static python::tuple getinitargs(const SubstructLibrary &self) {
    std::string res;
    if (!SubstructLibraryCanSerialize()) {
      throw_runtime_error("Pickling of FilterCatalog instances is not enabled");
    }
    res = self.Serialize();
    return python::make_tuple(python::object(python::handle<>(
        PyBytes_FromStringAndSize(res.c_str(), res.length()))));
  };
};

void toStream(const SubstructLibrary &cat, python::object &fileobj) {
  streambuf ss(fileobj, 't');
  streambuf::ostream ost(ss);
  cat.toStream(ost);
}

void initFromStream(SubstructLibrary &cat, python::object &fileobj) {
  streambuf ss(fileobj,
               'b');  // python StringIO can't seek, so need binary data
  streambuf::istream is(ss);
  cat.initFromStream(is);
}

boost::shared_ptr<MolHolderBase> GetMolHolder(SubstructLibrary &sslib) {
  // need to convert from a ref to a real shared_ptr
  return sslib.getMolHolder();
}

boost::shared_ptr<FPHolderBase> GetFpHolder(SubstructLibrary &sslib) {
  // need to convert from a ref to a real shared_ptr
  return sslib.getFpHolder();
}

struct substructlibrary_wrapper {
  static void wrap() {
    python::class_<MolHolderBase, boost::shared_ptr<MolHolderBase>,
                   boost::noncopyable>("MolHolderBase", "", python::no_init)
        .def("__len__", &MolHolderBase::size)
        .def("AddMol", &MolHolderBase::addMol,
             "Adds molecule to the molecule holder")
        .def("GetMol", &MolHolderBase::getMol,
             "Returns a particular molecule in the molecule holder\n\n"
             "  ARGUMENTS:\n"
             "    - idx: which molecule to return\n\n"
             "    - sanitize: if sanitize is False, return the internal "
             "molecule state [default True]\n\n"
             "  NOTE: molecule indices start at 0\n")
        .def("__len__", &MolHolderBase::size);

    python::class_<MolHolder, boost::shared_ptr<MolHolder>,
                   python::bases<MolHolderBase>>("MolHolder", MolHolderDoc,
                                                 python::init<>());

    python::class_<CachedMolHolder, boost::shared_ptr<CachedMolHolder>,
                   python::bases<MolHolderBase>>(
        "CachedMolHolder", CachedMolHolderDoc, python::init<>())
        .def("AddBinary", &CachedMolHolder::addBinary, (python::args("pickle")),
             "Add a binary pickle to the molecule holder, no checking is done "
             "on the input data");

    python::class_<CachedSmilesMolHolder,
                   boost::shared_ptr<CachedSmilesMolHolder>,
                   python::bases<MolHolderBase>>(
        "CachedSmilesMolHolder", CachedSmilesMolHolderDoc, python::init<>())
        .def("AddSmiles", &CachedSmilesMolHolder::addSmiles,
             (python::args("smiles")),
             "Add a trusted smiles string to the molecule holder, no checking "
             "is done on the input data");

    python::class_<CachedTrustedSmilesMolHolder,
                   boost::shared_ptr<CachedTrustedSmilesMolHolder>,
                   python::bases<MolHolderBase>>(
        "CachedTrustedSmilesMolHolder", CachedTrustedSmilesMolHolderDoc,
        python::init<>())
        .def("AddSmiles", &CachedTrustedSmilesMolHolder::addSmiles,
             (python::args("smiles")),
             "Add a trusted smiles string to the molecule holder, no checking "
             "is done on the input data");

    python::class_<FPHolderBase, boost::shared_ptr<FPHolderBase>,
                   boost::noncopyable>("FPHolderBase", "", python::no_init)
        .def("__len__", &FPHolderBase::size)

        .def("AddMol", &FPHolderBase::addMol,
             "Adds a molecule to the fingerprint database, returns the index "
             "of the new pattern")
        .def("AddFingerprint",
             (unsigned int (FPHolderBase::*)(const ExplicitBitVect &)) &
                 FPHolderBase::addFingerprint,
             "Adds a raw bit vector to the fingerprint database, returns the "
             "index of the supplied pattern")
        .def("GetFingerprint", &FPHolderBase::getFingerprint,
             python::return_value_policy<python::reference_existing_object>(),
             "Return the bit vector at the specified index")
        .def("PassesFilter", &FPHolderBase::passesFilter,
             (python::args("idx"), python::args("query")),
             "Returns True if the specified index passes the filter supplied "
             "by the query bit vector")
        .def("MakeFingerprint", &FPHolderBase::makeFingerprint,
             (python::arg("mol")),
             python::return_value_policy<python::manage_new_object>(),
             "Compute the query bits for the holder");

    python::class_<PatternHolder, boost::shared_ptr<PatternHolder>,
                   python::bases<FPHolderBase>>(
        "PatternHolder", PatternHolderDoc, python::init<>())
        .def(python::init<unsigned int>());

    python::class_<TautomerPatternHolder,
                   boost::shared_ptr<TautomerPatternHolder>,
                   python::bases<FPHolderBase>>(
        "TautomerPatternHolder", TautomerPatternHolderDoc, python::init<>())
        .def(python::init<unsigned int>());

    python::class_<SubstructLibrary, SubstructLibrary *,
                   const SubstructLibrary *>(
        "SubstructLibrary", SubstructLibraryDoc, python::init<>())
        .def(python::init<boost::shared_ptr<MolHolderBase>>())
        .def(python::init<boost::shared_ptr<MolHolderBase>,
                          boost::shared_ptr<FPHolderBase>>())
        .def(python::init<std::string>())

        .def("GetMolHolder", &GetMolHolder)
        .def("GetFpHolder", &GetFpHolder)

        .def("AddMol", &SubstructLibrary::addMol, (python::arg("mol")),
             "Adds a molecule to the substruct library")

        .def("GetMatches",
             (std::vector<unsigned int>(SubstructLibrary::*)(
                 const ROMol &, bool, bool, bool, int, int) const) &
                 SubstructLibrary::getMatches,
             (python::arg("query"), python::arg("recursionPossible") = true,
              python::arg("useChirality") = true,
              python::arg("useQueryQueryMatches") = false,
              python::arg("numThreads") = -1, python::arg("maxResults") = 1000),
             "Get the matches for the query.\n\n"
             " Arguments:\n"
             "  - query:      substructure query\n"
             "  - numThreads: number of threads to use, -1 means all threads\n"
             "  - maxResults: maximum number of results to return")

        .def("GetMatches",
             (std::vector<unsigned int>(SubstructLibrary::*)(
                 const ROMol &, unsigned int, unsigned int, bool, bool, bool,
                 int, int) const) &
                 SubstructLibrary::getMatches,
             (python::arg("query"), python::arg("startIdx"),
              python::arg("endIdx"), python::arg("recursionPossible") = true,
              python::arg("useChirality") = true,
              python::arg("useQueryQueryMatches") = false,
              python::arg("numThreads") = -1, python::arg("maxResults") = 1000),
             "Get the matches for the query.\n\n"
             " Arguments:\n"
             "  - query:      substructure query\n"
             "  - startIdx:   index to search from\n"
             "  - endIdx:     index (non-inclusize) to search to\n"
             "  - numThreads: number of threads to use, -1 means all threads\n"
             "  - maxResults: maximum number of results to return")

        .def("CountMatches",
             (unsigned int (SubstructLibrary::*)(const ROMol &, bool, bool,
                                                 bool, int) const) &
                 SubstructLibrary::countMatches,
             (python::arg("query"), python::arg("recursionPossible") = true,
              python::arg("useChirality") = true,
              python::arg("useQueryQueryMatches") = false,
              python::arg("numThreads") = -1),
             "Get the matches for the query.\n\n"
             " Arguments:\n"
             "  - query:      substructure query\n"
             "  - numThreads: number of threads to use, -1 means all threads\n")

        .def("CountMatches",
             (unsigned int (SubstructLibrary::*)(const ROMol &, unsigned int,
                                                 unsigned int, bool, bool, bool,
                                                 int) const) &
                 SubstructLibrary::countMatches,
             (python::arg("query"), python::arg("startIdx"),
              python::arg("endIdx"), python::arg("recursionPossible") = true,
              python::arg("useChirality") = true,
              python::arg("useQueryQueryMatches") = false,
              python::arg("numThreads") = -1),
             "Get the matches for the query.\n\n"
             " Arguments:\n"
             "  - query:      substructure query\n"
             "  - startIdx:   index to search from\n"
             "  - endIdx:     index (non-inclusize) to search to\n"
             "  - numThreads: number of threads to use, -1 means all threads\n")

        .def("HasMatch",
             (bool (SubstructLibrary::*)(const ROMol &, bool, bool, bool, int)
                  const) &
                 SubstructLibrary::hasMatch,
             (python::arg("query"), python::arg("recursionPossible") = true,
              python::arg("useChirality") = true,
              python::arg("useQueryQueryMatches") = false,
              python::arg("numThreads") = -1),
             "Get the matches for the query.\n\n"
             " Arguments:\n"
             "  - query:      substructure query\n"
             "  - numThreads: number of threads to use, -1 means all threads\n")

        .def("HasMatch",
             (bool (SubstructLibrary::*)(const ROMol &, unsigned int,
                                         unsigned int, bool, bool, bool, int)
                  const) &
                 SubstructLibrary::hasMatch,
             (python::arg("query"), python::arg("startIdx"),
              python::arg("endIdx"), python::arg("recursionPossible") = true,
              python::arg("useChirality") = true,
              python::arg("useQueryQueryMatches") = false,
              python::arg("numThreads") = -1),
             "Get the matches for the query.\n\n"
             " Arguments:\n"
             "  - query:      substructure query\n"
             "  - startIdx:   index to search from\n"
             "  - endIdx:     index (non-inclusize) to search to\n"
             "  - numThreads: number of threads to use, -1 means all threads\n")
        // =========================================================================
        // TautomerQueries
        .def("GetMatches",
             (std::vector<unsigned int>(SubstructLibrary::*)(
                 const TautomerQuery &, bool, bool, bool, int, int) const) &
                 SubstructLibrary::getMatches,
             (python::arg("query"), python::arg("recursionPossible") = true,
              python::arg("useChirality") = true,
              python::arg("useQueryQueryMatches") = false,
              python::arg("numThreads") = -1, python::arg("maxResults") = 1000),
             "Get the matches for the query.\n\n"
             " Arguments:\n"
             "  - query:      tautomer query\n"
             "  - numThreads: number of threads to use, -1 means all threads\n"
             "  - maxResults: maximum number of results to return")

        .def("GetMatches",
             (std::vector<unsigned int>(SubstructLibrary::*)(
                 const TautomerQuery &, unsigned int, unsigned int, bool, bool,
                 bool, int, int) const) &
                 SubstructLibrary::getMatches,
             (python::arg("query"), python::arg("startIdx"),
              python::arg("endIdx"), python::arg("recursionPossible") = true,
              python::arg("useChirality") = true,
              python::arg("useQueryQueryMatches") = false,
              python::arg("numThreads") = -1, python::arg("maxResults") = 1000),
             "Get the matches for the query.\n\n"
             " Arguments:\n"
             "  - query:      tautomer query\n"
             "  - startIdx:   index to search from\n"
             "  - endIdx:     index (non-inclusize) to search to\n"
             "  - numThreads: number of threads to use, -1 means all threads\n"
             "  - maxResults: maximum number of results to return")

        .def("CountMatches",
             (unsigned int (SubstructLibrary::*)(const TautomerQuery &, bool,
                                                 bool, bool, int) const) &
                 SubstructLibrary::countMatches,
             (python::arg("query"), python::arg("recursionPossible") = true,
              python::arg("useChirality") = true,
              python::arg("useQueryQueryMatches") = false,
              python::arg("numThreads") = -1),
             "Get the matches for the query.\n\n"
             " Arguments:\n"
             "  - query:      tautomer query\n"
             "  - numThreads: number of threads to use, -1 means all threads\n")

        .def("CountMatches",
             (unsigned int (SubstructLibrary::*)(const TautomerQuery &,
                                                 unsigned int, unsigned int,
                                                 bool, bool, bool, int) const) &
                 SubstructLibrary::countMatches,
             (python::arg("query"), python::arg("startIdx"),
              python::arg("endIdx"), python::arg("recursionPossible") = true,
              python::arg("useChirality") = true,
              python::arg("useQueryQueryMatches") = false,
              python::arg("numThreads") = -1),
             "Get the matches for the query.\n\n"
             " Arguments:\n"
             "  - query:      tautomer query\n"
             "  - startIdx:   index to search from\n"
             "  - endIdx:     index (non-inclusize) to search to\n"
             "  - numThreads: number of threads to use, -1 means all threads\n")

        .def("HasMatch",
             (bool (SubstructLibrary::*)(const TautomerQuery &, bool, bool,
                                         bool, int) const) &
                 SubstructLibrary::hasMatch,
             (python::arg("query"), python::arg("recursionPossible") = true,
              python::arg("useChirality") = true,
              python::arg("useQueryQueryMatches") = false,
              python::arg("numThreads") = -1),
             "Get the matches for the query.\n\n"
             " Arguments:\n"
             "  - query:      tautomer query\n"
             "  - numThreads: number of threads to use, -1 means all threads\n")

        .def("HasMatch",
             (bool (SubstructLibrary::*)(const TautomerQuery &, unsigned int,
                                         unsigned int, bool, bool, bool, int)
                  const) &
                 SubstructLibrary::hasMatch,
             (python::arg("query"), python::arg("startIdx"),
              python::arg("endIdx"), python::arg("recursionPossible") = true,
              python::arg("useChirality") = true,
              python::arg("useQueryQueryMatches") = false,
              python::arg("numThreads") = -1),
             "Get the matches for the query.\n\n"
             " Arguments:\n"
             "  - query:      tautomer query\n"
             "  - startIdx:   index to search from\n"
             "  - endIdx:     index (non-inclusize) to search to\n"
             "  - numThreads: number of threads to use, -1 means all threads\n")

        .def("GetMol", &SubstructLibrary::getMol,
             "Returns a particular molecule in the molecule holder\n\n"
             "  ARGUMENTS:\n"
             "    - idx: which molecule to return\n\n"
             "  NOTE: molecule indices start at 0\n")

        .def("__len__", &SubstructLibrary::size)

        .def("ToStream", &toStream, python::arg("stream"),
             "Serialize a substructure library to a python text stream.\n"
             "The stream can be a file in text mode or an io.StringIO type "
             "object\n\n"
             "  ARGUMENTS:\n"
             "    - stream: a text or text stream like object\n\n"
             "  >>> from rdkit.Chem import rdSubstructLibrary\n"
             "  >>> import io\n"
             "  >>> lib = rdSubstructLibrary.SubstructLibrary()\n"
             "  >>> stream = io.StringIO()\n"
             "  >>> lib.ToStream(stream)\n\n"
             "   or\n"
             "  >>> with open('rdkit.sslib', 'w') as stream:\n"
             "  ...  lib.ToStream(stream)\n")

        .def("InitFromStream", &initFromStream, python::arg("stream"),
             "Deserialize a substructure library from a python bytes stream.\n"
             "Python doesn't allow seeking operations inside a unicode or "
             "string stream anymore\n"
             "so this requires opening a file in binary mode or using an "
             "io.ByteIO type object\n\n"
             "  ARGUMENTS:\n"
             "    - stream: a binary stream like object\n\n"
             "  SubstructLibrary.Serialize already writes a binary stream\n\n"
             "  >>> from rdkit.Chem import rdSubstructLibrary\n"
             "  >>> import io\n"
             "  >>> lib = rdSubstructLibrary.SubstructLibrary()\n"
             "  >>> stream = io.BytesIO( lib.Serialize() )\n"
             "  >>> lib.InitFromStream(stream)\n\n"
             "   remember to write to text and read from a binary stream\n"
             "  >>> with open('rdkit.sslib', 'w') as f: lib.ToStream(f)\n"
             "  >>> with open('rdkit.sslib', 'rb') as f: "
             "lib.InitFromStream(f)\n")

        .def("Serialize", &SubstructLibrary_Serialize)
        // enable pickle support
        .def_pickle(substructlibrary_pickle_suite());

    python::def("SubstructLibraryCanSerialize", SubstructLibraryCanSerialize,
                "Returns True if the SubstructLibrary is serializable "
                "(requires boost serialization");

    python::def("AddPatterns",
                (void (*)(SubstructLibrary &, int)) & addPatterns,
                "Add pattern fingerprints to the given library, use "
                "numThreads=-1 to use all available cores",
                (python::arg("sslib"), python::arg("numThreads") = 1));

    python::def(
        "AddPatterns",
        (void (*)(SubstructLibrary &, boost::shared_ptr<FPHolderBase>, int)) &
            addPatterns,
        "Add pattern fingerprints to the given library, use numThreads=-1 to "
        "use all available cores",
        (python::arg("sslib"), python::arg("patterns"),
         python::arg("numThreads") = 1));
  }
};
}  // namespace RDKit

void wrap_substructlibrary() { RDKit::substructlibrary_wrapper::wrap(); }
