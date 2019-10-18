// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_PARAMS_H
#define BITCOIN_CONSENSUS_PARAMS_H

#include <amount.h>
#include <uint256.h>

#include <limits>
#include <map>
#include <set>
#include <string>

namespace Consensus {

enum DeploymentPos
{
    DEPLOYMENT_TESTDUMMY,
    DEPLOYMENT_CSV, // Deployment of BIP68, BIP112, and BIP113.
    DEPLOYMENT_SEGWIT, // Deployment of BIP141, BIP143, and BIP147.
    // NOTE: Also add new deployments to VersionBitsDeploymentInfo in versionbits.cpp
    MAX_VERSION_BITS_DEPLOYMENTS
};

/**
 * Struct for each individual consensus rule change using BIP9.
 */
struct BIP9Deployment {
    /** Bit position to select the particular bit in nVersion. */
    int bit;
    /** Start MedianTime for version bits miner confirmation. Can be a date in the past */
    int64_t nStartTime;
    /** Timeout/expiry MedianTime for the deployment attempt. */
    int64_t nTimeout;

    /** Constant for nTimeout very far in the future. */
    static constexpr int64_t NO_TIMEOUT = std::numeric_limits<int64_t>::max();

    /** Special value for nStartTime indicating that the deployment is always active.
     *  This is useful for testing, as it means tests don't need to deal with the activation
     *  process (which takes at least 3 BIP9 intervals). Only tests that specifically test the
     *  behaviour during activation cannot use this. */
    static constexpr int64_t ALWAYS_ACTIVE = -1;
};

/**
 * Parameters that influence chain consensus.
 */
struct Params {
    /** BitcoinHD Fund address */
    std::string xdiskFundAddress;
    std::set<std::string> xdiskFundAddressPool;

    uint256 hashGenesisBlock;
    /** Subsidy halving interval blocks base on 600 seconds */
    int nSubsidyHalvingInterval;
    int nCapacityEvalWindow;

    /** xdiskIP = BitcoinHD Improvement Proposals, like BIP */
    /** BitcoinHD target spacing */
    int xdiskIP001TargetSpacing;
    /** BitcoinHD fund pre-mining height */
    int xdiskIP001PreMiningEndHeight;
    /** BitcoinHD fund zero height */
    int xdiskIP001FundZeroLastHeight;
    /** BitcoinHD fund royalty for full mortgage. 1000% */
    int xdiskIP001FundRoyaltyForFullMortgage;
    /** BitcoinHD fund royalty for low mortgage. 1000% */
    int xdiskIP001FundRoyaltyForLowMortgage;
    /** BitcoinHD miner mining ratio per TB */
    CAmount xdiskIP001MiningRatio;

    /** View all xdiskIP document on https://xdisk.org/wiki/xdiskIP */
    /** Block height at which xdiskIP004 becomes active */
    int xdiskIP004Height;
    /** Block height at which xdiskIP004 becomes inactive */
    int xdiskIP004AbandonHeight;

    /** Block height at which xdiskIP006 becomes active */
    int xdiskIP006Height;
    /** Block height at which xdiskIP006 bind plotter becomes active */
    int xdiskIP006BindPlotterActiveHeight;
    int xdiskIP006CheckRelayHeight;
    int xdiskIP006LimitBindPlotterHeight;

    /** Block height at which xdiskIP007 becomes active */
    int xdiskIP007Height;
    int xdiskIP007SmoothEndHeight;
    int64_t xdiskIP007MiningRatioStage;

    /** Block height at which xdiskIP008 becomes active */
    int xdiskIP008Height;
    int xdiskIP008TargetSpacing;
    int xdiskIP008FundRoyaltyForLowMortgage;
    int xdiskIP008FundRoyaltyDecreaseForLowMortgage;
    int xdiskIP008FundRoyaltyDecreasePeriodForLowMortgage;

    /**
     * Minimum blocks including miner confirmation of the total of 2016 blocks in a retargeting period,
     * (nPocTargetTimespan / xdiskIP001TargetSpacing) which is also used for BIP9 deployments.
     * Examples: 1916 for 95%, 1512 for testchains.
     */
    int nRuleChangeActivationThreshold;
    int nMinerConfirmationWindow;
    BIP9Deployment vDeployments[MAX_VERSION_BITS_DEPLOYMENTS];

    /** Proof of Capacity parameters */
    bool fAllowMinDifficultyBlocks;
    uint256 nMinimumChainWork;
    uint256 defaultAssumeValid;

    /** Block height at which BIP16 becomes active */
    int BIP16Height;
    /** Block height and hash at which BIP34 becomes active */
    int BIP34Height;
    /** Block height at which BIP65 becomes active */
    int BIP65Height;
    /** Block height at which BIP66 becomes active */
    int BIP66Height;
};

// Get target time space
inline int GetTargetSpacing(int nHeight, const Params& params) {
    return nHeight >= params.xdiskIP008Height ? params.xdiskIP008TargetSpacing : params.xdiskIP001TargetSpacing;
}

} // namespace Consensus

#endif // BITCOIN_CONSENSUS_PARAMS_H
