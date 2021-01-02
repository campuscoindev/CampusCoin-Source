// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "main.h"
#include "uint256.h"

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    // How many times we expect transactions after the last checkpoint to
    // be slower. This number is a compromise, as it can't be accurate for
    // every system. When reindexing from a fast disk with a slow CPU, it
    // can be up to 20, while when downloading from a slow network with a
    // fast multicore CPU, it won't be much higher than 1.
    static const double fSigcheckVerificationFactor = 5.0;


    struct CCheckpointData {
        const MapCheckpoints *mapCheckpoints;
        int64 nTimeLastCheckpoint;
        int64 nTransactionsLastCheckpoint;
        double fTransactionsPerDay;
    };

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
               ( 0,       uint256("0x2cadd7467aa4ca55a8be084aa8779da4f250981cbd7a7e377a4502bb58e5578b"))
			   ( 1,       uint256("0x4f8e827684f70fb6d731c92a401572e180d0ebc35932fc617e9c717cdee7e9b0"))
			   ( 2,       uint256("0xc388466de0b8cc23e70c779afdd35e2f90bac606193a54b6c99b0be789189a8e"))
               ( 500,     uint256("0x5cf4d300bce5076aa32a2ddf2376c4be335d9656b054c0433124cd2402ed7c48"))
               ( 5000,    uint256("0x5299233cbc9ef9793a43ef239471a1e7f01ce8262c2d0e1c74bf4a9be1026919"))
               ( 25000,   uint256("0xf423e43134b7c014f5e5d8b8db999bdd6df7d2b038255f9520bb2c3073c55110"))
               ( 50000,   uint256("0xef707bfb28061129e4c0aeda5f8e8fd8fb9f10df42ccc51e51ff17b8b7563da7"))
               ( 100000,  uint256("0x16f93f7e3974422e1a68bd8a6eb3f2d5776e7db72b8cc8a08946081cac253dce"))
               ( 125000,  uint256("0x8bd440c1238c76264734ea05436454fd3873fb5acce1684c042031a6ee18cddf"))
               ( 500000,  uint256("0xf57dde1116078c96c2f4250205cbb61d17b2bf9045a7fa008881d153937f0492"))
               ( 750000,  uint256("0x6c4bd350bda75f32ab746c7ad78752881c821ce7cdb7835ca5535d2c455b1144"))
               ( 1000000, uint256("0xa71fdfa33f4278974dc99fd2b41a73b285fb8807c6ae474184429adc4599a8a5"))
               ( 1250000, uint256("0x13a323e1005cb3486f3b1b277181371cbb450cf820d2e77e0fa9600716cad324"))
               ( 1500000, uint256("0x24f1fc28a50fc57d37a68cc5e3a0d957047c9f7170d74ea036b7627414ce4df1")) 
	;
    static const CCheckpointData data = {
        &mapCheckpoints,
        1388880557, // * UNIX timestamp of last checkpoint block
        0,    // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        8000.0     // * estimated number of transactions per day after checkpoint
    };

    static MapCheckpoints mapCheckpointsTestnet = 
        boost::assign::map_list_of
        (     0, uint256("0x"))
        ;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1369685559,
        37581,
        300
    };

    const CCheckpointData &Checkpoints() {
        if (fTestNet)
            return dataTestnet;
        else
            return data;
    }

    bool CheckBlock(int nHeight, const uint256& hash)
    {
        if (fTestNet) return true; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return true;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    // Guess how far we are in the verification process at the given block index
    double GuessVerificationProgress(CBlockIndex *pindex) {
        if (pindex==NULL)
            return 0.0;

        int64 nNow = time(NULL);

        double fWorkBefore = 0.0; // Amount of work done before pindex
        double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
        // Work is defined as: 1.0 per transaction before the last checkoint, and
        // fSigcheckVerificationFactor per transaction after.

        const CCheckpointData &data = Checkpoints();

        if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
            double nCheapBefore = pindex->nChainTx;
            double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
            double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore;
            fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
        } else {
            double nCheapBefore = data.nTransactionsLastCheckpoint;
            double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
            double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
            fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
        }

        return fWorkBefore / (fWorkBefore + fWorkAfter);
    }

    int GetTotalBlocksEstimate()
    {
        if (fTestNet) return 0; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return 0;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (fTestNet) return NULL; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return NULL;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }
}
