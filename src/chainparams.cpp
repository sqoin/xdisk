// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin  Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>
#include <chainparamsseeds.h>
#include <consensus/consensus.h>
#include <consensus/merkle.h>
#include <poc/poc.h>
#include <script/interpreter.h>
#include <tinyformat.h>
#include <util.h>
#include <utilstrencodings.h>

#include <cassert>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint64_t nNonce, uint64_t nBaseTarget, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(2);
    txNew.vin[0].scriptSig = CScript() << static_cast<unsigned int>(0)
        << CScriptNum(static_cast<int64_t>(nNonce)) << CScriptNum(static_cast<int64_t>(0))
        << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;
    txNew.vout[1].nValue = 0;
    txNew.vout[1].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime       = nTime;
    genesis.nBaseTarget = nBaseTarget;
    genesis.nNonce      = nNonce;
    genesis.nVersion    = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=8cec494f7f02ad, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=6b80acabaf0fef, nTime=1531292789, nBaseTarget=18325193796, nNonce=0, vtx=1)
 *   CTransaction(hash=6b80acabaf0fef, ver=1, vin.size=1, vout.size=2, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=25.00000000, scriptPubKey=0x2102CD2103A86877937A05)
 *     CTxOut(nValue=00.00000000, scriptPubKey=0x2102CD2103A86877937A05)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint64_t nNonce, uint64_t nBaseTarget, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
    const CScript genesisOutputScript = CScript() << ParseHex("02cd2103a86877937a05eff85cf487424b52796542149f2888f9a17fbe6d66ce9d") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBaseTarget, nVersion, genesisReward);
}

void CChainParams::UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    consensus.vDeployments[d].nStartTime = nStartTime;
    consensus.vDeployments[d].nTimeout = nTimeout;
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";

        consensus.xdiskFundAddress = "32B86ghqRTJkh2jvyhRWFugX7YWoqHPqVE";
        // See https://xdisk.org/wiki/fund-address-pool
        consensus.xdiskFundAddressPool = {
            "3F26JRhiGjc8z8pRKJvLXBEkdE6nLDAA3y", //!< 0x20000000, Deprecated!. Last use on v1.1.0.1-30849da
            "32B86ghqRTJkh2jvyhRWFugX7YWoqHPqVE", //!< 0x20000004, 0x20000000
            "39Vb1GNSurGoHcQ4aTKrTYC1oNmPppGea3",
            "3Maw3PdwSvtXgBKJ9QPGwRSQW8AgQrGK3W",
            "3Hy3V3sPVpuQaG6ttihfQNh4vcDXumLQq9",
            "3MxgS9jRcGLihAtb9goAyD1QC8AfRNFE1F",
            "3A4uNFxQf6Jo8b6QpBVnNcjDRqDchgpGbR",
        };
        assert(consensus.xdiskFundAddressPool.find(consensus.xdiskFundAddress) != consensus.xdiskFundAddressPool.end());

        consensus.nCapacityEvalWindow            = 2016;   // About 1 week
        consensus.nSubsidyHalvingInterval        = 210240; // About 4 years. 210000*600/(365*24*3600) = 3.99543379
        consensus.fAllowMinDifficultyBlocks      = false;  // For test
        consensus.nRuleChangeActivationThreshold = 1916;   // 95% of 2016
        consensus.nMinerConfirmationWindow       = 2016;   // About 1 week

        consensus.xdiskIP001PreMiningEndHeight         = 84001; // 21M * 10% = 2.1M, 2.1M/25=84000 (+1 for deprecated public test data)
        consensus.xdiskIP001FundZeroLastHeight         = 92641; // End 1 month after 30 * 24 * 60 / 5 = 8640
        consensus.xdiskIP001TargetSpacing              = 300;   // 5 minutes. Subsidy halving interval 420000 blocks
        consensus.xdiskIP001FundRoyaltyForFullMortgage = 50;    // 50‰ to fund
        consensus.xdiskIP001FundRoyaltyForLowMortgage  = 700;   // 700‰ to fund
        consensus.xdiskIP001MiningRatio                = 3 * COIN;

        // It's fuck mind Bitcoin HD Improvement Proposals
        consensus.xdiskIP004Height        = 96264; // Bitcoin HD new consensus upgrade bug. 96264 is first invalid block
        consensus.xdiskIP004AbandonHeight = 99000;

        consensus.xdiskIP006Height                  = 129100; // Actived on Wed, 02 Jan 2019 02:17:19 GMT
        consensus.xdiskIP006BindPlotterActiveHeight = 131116; // Bind plotter actived on Tue, 08 Jan 2019 23:14:57 GMT
		consensus.xdiskIP006CheckRelayHeight        = 133000; // Bind and unbind plotter limit. Active on Tue, 15 Jan 2019 11:00:00 GMT
        consensus.xdiskIP006LimitBindPlotterHeight  = 134650; // Bind plotter limit. Active on Tue, 21 Jan 2019 9:00:00 GMT

        consensus.xdiskIP007Height           = 168300; // Begin xdiskIP007 consensus
        consensus.xdiskIP007SmoothEndHeight  = 172332; // 240 -> 300, About 2 weeks
        consensus.xdiskIP007MiningRatioStage = 1250 * 1024; // 1250 PB

        consensus.xdiskIP008Height                                  = 197568; // Begin xdiskIP008 consensus. About active on Tue, 27 Aug 2019 04:47:46 GMT
        consensus.xdiskIP008TargetSpacing                           = 180;    // 3 minutes. Subsidy halving interval 700000 blocks
        consensus.xdiskIP008FundRoyaltyForLowMortgage               = 270;    // 270‰ to fund
        consensus.xdiskIP008FundRoyaltyDecreaseForLowMortgage       = 20;     // 20‰ decrease
        consensus.xdiskIP008FundRoyaltyDecreasePeriodForLowMortgage = 33600;  // 10 weeks. About 110 weeks decrease to 50‰
        assert(consensus.xdiskIP008Height % consensus.nMinerConfirmationWindow == 0);
        assert(consensus.xdiskIP008FundRoyaltyForLowMortgage < consensus.xdiskIP001FundRoyaltyForLowMortgage);
        assert(consensus.xdiskIP008FundRoyaltyForLowMortgage > consensus.xdiskIP001FundRoyaltyForFullMortgage);

        consensus.BIP16Height = 0; // Always enforce BIP16
        consensus.BIP34Height = 0; // Always enforce BIP34
        consensus.BIP65Height = 0; // Always enforce BIP65
        consensus.BIP66Height = 0; // Always enforce BIP66

        // TestDummy
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000001c422e58c266a7b7d31");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x6625f6c687d4f58572f1207ebed1953f5f20c63c5fdc3d59cc14222de1a05a1f");

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x1a;
        pchMessageStart[1] = 0xcb;
        pchMessageStart[2] = 0x5d;
        pchMessageStart[3] = 0xbb;
        nDefaultPort = 8177;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1531292789, 0, poc::GetBaseTarget(240), 2, 50 * COIN * consensus.xdiskIP001TargetSpacing / 600);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x8cec494f7f02ad25b3abf418f7d5647885000e010c34e16c039711e4061497b0"));
        assert(genesis.hashMerkleRoot == uint256S("0x6b80acabaf0fef45e2cad0b8b63d07cff1b35640e81f3ab3d83120dd8bc48164"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as a oneshot if they dont support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.push_back("seed0-chain.xdisk.org");
        vSeeds.push_back("seed1-chain.xdisk.org");
        vSeeds.push_back("seed2-chain.xdisk.org");
        vSeeds.push_back("seed3-chain.xdisk.org");
        vSeeds.push_back("seed-xdisk.hpool.com");
        vSeeds.push_back("seed-xdisk.hdpool.com");
        vSeeds.push_back("seed-xdisk.awpool.com");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,0);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp = "bc";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;

        checkpointData = {
            {
            }
        };

        chainTxData = ChainTxData{
            // Data from getchaintxstats
            1566989688, // * UNIX timestamp of last known number of transactions
            294357,     // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0.007077    // * estimated number of transactions per second after that timestamp
        };
    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";

        consensus.xdiskFundAddress = "2N3DHXpYQFZ6pNCUxNpHuTtaFQZJCmCKNBw";
        consensus.xdiskFundAddressPool = { "2N3DHXpYQFZ6pNCUxNpHuTtaFQZJCmCKNBw" };
        assert(consensus.xdiskFundAddressPool.find(consensus.xdiskFundAddress) != consensus.xdiskFundAddressPool.end());

        consensus.nCapacityEvalWindow            = 2016;
        consensus.nSubsidyHalvingInterval        = 210000;
        consensus.fAllowMinDifficultyBlocks      = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow       = 2016;

        consensus.xdiskIP001PreMiningEndHeight         = 8400; // 21M * 1% = 0.21M, 0.21M/25=8400
        consensus.xdiskIP001FundZeroLastHeight         = 12400;
        consensus.xdiskIP001TargetSpacing              = 300;
        consensus.xdiskIP001FundRoyaltyForFullMortgage = 50;  // 50‰
        consensus.xdiskIP001FundRoyaltyForLowMortgage  = 700; // 700‰
        consensus.xdiskIP001MiningRatio                = 3 * COIN;

        consensus.xdiskIP004Height        = 12400; // xdiskIP004. Bitcoin HD new consensus upgrade bug.
        consensus.xdiskIP004AbandonHeight = 21000;

        consensus.xdiskIP006Height                  = 41290;
        consensus.xdiskIP006BindPlotterActiveHeight = 41296;
        consensus.xdiskIP006CheckRelayHeight        = 45328;
        consensus.xdiskIP006LimitBindPlotterHeight  = 48790;

        consensus.xdiskIP007Height           = 72550;
        consensus.xdiskIP007SmoothEndHeight  = 76582; // 240 -> 300, About 2 weeks
        consensus.xdiskIP007MiningRatioStage = 10;    // 10 TB

        consensus.xdiskIP008Height                                  = 106848; // About active on Fri, 09 Aug 2019 10:01:58 GMT
        consensus.xdiskIP008TargetSpacing                           = 180;
        consensus.xdiskIP008FundRoyaltyForLowMortgage               = 270;  // 270‰ to fund
        consensus.xdiskIP008FundRoyaltyDecreaseForLowMortgage       = 20;   // 20‰ decrease
        consensus.xdiskIP008FundRoyaltyDecreasePeriodForLowMortgage = 1008; // About half week
        assert(consensus.xdiskIP008Height % consensus.nMinerConfirmationWindow == 0);
        assert(consensus.xdiskIP008FundRoyaltyForLowMortgage < consensus.xdiskIP001FundRoyaltyForLowMortgage);
        assert(consensus.xdiskIP008FundRoyaltyForLowMortgage > consensus.xdiskIP001FundRoyaltyForFullMortgage);

        consensus.BIP16Height = 0; // Always enforce BIP16
        consensus.BIP34Height = 0; // Always enforce BIP34
        consensus.BIP65Height = 0; // Always enforce BIP65
        consensus.BIP66Height = 0; // Always enforce BIP66

        // TestDummy
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000000002e8d8e882e64cdd");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x87facc4009fa5eef8d7d50168eae18ca0e8a9ce9a0c5d41fb828c2058553c497");

        pchMessageStart[0] = 0x1e;
        pchMessageStart[1] = 0x12;
        pchMessageStart[2] = 0xa0;
        pchMessageStart[3] = 0x08;
        nDefaultPort = 18733;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1531292789, 1, poc::GetBaseTarget(240), 2, 50 * COIN * consensus.xdiskIP001TargetSpacing / 600);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0xb67faee747224b7646d66cd08763f33d72b594da8e884535c2f95904fe3cf8c1"));
        assert(genesis.hashMerkleRoot == uint256S("0xb8f17dd05a0d3fe40963d189ee0397ff909ce33bd1c9821898d2400b89ea75e6"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.push_back("testnet-seed0-chain.xdisk.org");
        vSeeds.push_back("testnet-seed1-chain.xdisk.org");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tb";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;

        checkpointData = {
            {
                {      0, uint256S("0xb67faee747224b7646d66cd08763f33d72b594da8e884535c2f95904fe3cf8c1") },
                {   2000, uint256S("0x13c8cd68809c554d883015b263bbe0b5ab19f3589ea659505e5e59c2aad7d32c") },
                {   4000, uint256S("0x1feebf8cd1458170c531fca3cc46baff57765ad2b61695b0a6ba28b9fd467fab") },
                {   6000, uint256S("0x13303b079c3c651c682096b419abbf610c06a87cb0bd5b2ddd638b1da5ccb29d") },
                {   8000, uint256S("0x070ef2c57440d016b0a97b005a18811d2396ca2aa9a5f2458e844ac4f86923a8") },
                {   8400, uint256S("0x9ceb8e7eaceba6d78d66b6561e60f0196e0580d2e708f0076578d99035def0e4") },
                // Offset +1000. Sync batch by 1000
                {   8600, uint256S("0x85328fd04bf8ece91dbb0e5d494059517a579c09e1c00cb1699aa832de42f825") },
                {   9000, uint256S("0xf6d498706e76904cf7404f0d701d542755713d8e238af3f2aeea5c228b0c4d79") },
                {  10000, uint256S("0x8912d6c4b3666e16db738c5b9253760da60140c6f6da4677c3c65a922c562981") },
                {  11000, uint256S("0x0437ad4d012411f2c2866aa6f7a57d87cdafe88b24eae2c4035b49e9621abbbb") },
                {  12000, uint256S("0xf274f4f16024e66139babad61b953f6e670dcc7ce729519013169491324afa42") },
                {  13000, uint256S("0x3d24ad7d4fb9e8aea19f53a8438c7ab8fb11057b95b0da667d61d826d567b22a") },
                {  14000, uint256S("0x225060edd20c722111a96eb7f1947a9cca58755f524fc8c23a6d64fe7fc5be92") },
                {  15000, uint256S("0x280971724545fa94d13d3d638cac4f7652e55ae83abf43f2b91d87a692cade35") },
                {  16000, uint256S("0x32e35edd797596ce166609b58f6ee6b920f8e60bfc7f4131f98ae21faa8398b7") },
                {  17000, uint256S("0x37df13069ca3e02d06cfd5f20a4895f2300a4bec851c5a28bcba3176fb6bcc5c") },
                {  18000, uint256S("0xd0b4e5638db1d664dffc561a6c4c9f6bf691c376a36ccbfc412418c4aee03df1") },
                {  19000, uint256S("0x0f8132de13286db54e4a8e4ea173af29c9b959dbbdcc206e674c56e6532406cf") },
                {  20000, uint256S("0xd886caeac8c821716d0cab7a1ffc58836eca7e24193c0fb7cdb0ccf0f08f8eea") },
                {  21000, uint256S("0x276c4ebfbecfd90b9cad397f78907604569f772c91c223247b6f3354e71b0dcd") },
                {  22000, uint256S("0x54ebf72c827700d162290d8369321266d78443d43387ed4dacc6bcc8e8dee61c") },
                {  23000, uint256S("0x0b0885eb92f712cf502c6adeef8e13e7b96c50cecc0e6f9972844be2cd66a578") },
                {  24000, uint256S("0xb0f1ce1964d04acc6e0ce02db2c1874b83811d973d025b95b8f927c94ba622d6") },
                {  25000, uint256S("0xb754a47f674d6489a9c0c2723912f98fb25fccc09c344eba7d7b9e175abd5284") },
                {  26000, uint256S("0xbacd9dcc2147915a3e84b0b24addad37c3f57c1cea01ea0483220effeac2ed0f") },
                {  27000, uint256S("0xbf6521f94f267e1a81dc479324340e5065c569f38449556be418e5b0909b9c44") },
                {  28000, uint256S("0xd119dc0c48a3071ea2e1522fa758e85717c9a19fbab7542b3890b3076a13851a") },
                {  29000, uint256S("0x0af437183e3c1d4d1b4d7f47237ac7f504aa1194d6cf58c872841e9d31603302") },
                {  30000, uint256S("0xc8c0367622ced32dd3c4793d974e04d672d8f88115149445f37c6c5950bfe9b2") },
                {  31000, uint256S("0xc5d44ac59d740717fa04e1709666c9e98766c638b7d20fcb9d30bc8c59033a17") },
                {  32000, uint256S("0xb7f29370d35409bfaf79715785ec44d73cb653c34d80190dced08e19af0d800b") },
                {  33000, uint256S("0x21b8ac3f7d40232e4ffd913e91f9bda6f1834812df5806df9af58ad1b17b235b") },
                {  34000, uint256S("0x941e0ef9fe8f8c1e1a05fcb4644b655b659f5d5dc64a1d9f946e450bcdd64fd6") },
                {  35000, uint256S("0xafab1f8f1368a9787b42f41ebc442a99dde6649c36a8f38f17641f310562fd3a") },
                {  36000, uint256S("0x91f802d19f2d24e3ae85a1fc3e30dd2896bb2469410ed5532d9bb7811427e2b0") },
                {  37000, uint256S("0xf5c65d46e2b95bad3ca951cddd03cd77650870d39aa5f756a9ea9490d9040047") },
                {  38000, uint256S("0x4496f906d81728a522dc5e5e8c4784df4663da9b7063f1e5e6ffd1474ef61fec") },
                {  39000, uint256S("0x1a16d2bb21d44e0399badc33d5215b32ccaeccd77721af3bd61be66e1db2bfcd") },
                {  40000, uint256S("0x5e968add124371c1d5fe4cd2171d45d2c8a11dea1339eb1ade25cb9725a0a56b") },
                {  41000, uint256S("0xf5aba9e950da5e44d1a8c1d2bdd616fba62a6e6c226492436782ad7fda3bcd72") },
                {  42000, uint256S("0x181454802688c0899771b697a2b2cb8e3d291ac098b0f41899319dc9f0c39112") },
                {  43000, uint256S("0x6a47b08bf3c4e58b7a8bec4451092cd5ab79891a63390dda2bcd810735819aa3") },
                {  44000, uint256S("0x9d25ffd74c63f7c1d56d33f279aeb3be91c7d64be85b6f87d3a4defef8d72456") },
                {  45000, uint256S("0x2ff01e7481e7829da065aac48f420b57513ce46aa4e429391064c42dde66df15") },
                {  46000, uint256S("0x1052f78825e55d3eec69684d7d5fd09fde27f7d07a21978229a2b14cee13bb73") },
                {  47000, uint256S("0xd799b0e60cca52bc2c1dff4f4af5eabf8b66f1a290b8e89323fbcc2506d582b6") },
                {  48000, uint256S("0x22784d9a08f451f51b00c23b99396e131840b7d9f51fe7be50a74c2025c6bd36") },
                {  49000, uint256S("0x7439cd47ad9e4158deaf123ba81bc11aaf67467c7824884ac2f9b564dd55e709") },
                {  50000, uint256S("0x762e0c3c9baedf5b113f7253ca9091bf646e009cb322f4aaf720d5a3f889dc45") },
                {  51000, uint256S("0xad667d924328cece33c1a7982a0f2662b69c4c99804c629967e3251b31adb3b1") },
                {  52000, uint256S("0x7a11b8feafa4921bf32a43fb5fa2938f658fc57916cb2972237ed928e3073ee9") },
                {  53000, uint256S("0xb00e50860354f3b2d2e5c19f2ade4978163262ab6eddaa5618cb21bb44e33380") },
                {  54000, uint256S("0x121773f98a69b2be0b99a33e63b4246311da1939c23bc3d726e94d6fa6d88a13") },
                {  55000, uint256S("0xb501478578ad612e2b1763b76a08a8b5760617c138af2f9379618d8b2b342f6a") },
                {  56000, uint256S("0x9d55d5b3c1df040dc7504a1b36f60d6f11f89036d8ac5869b82df67cb223edb3") },
                {  57000, uint256S("0xa7b978bd0aadf9743cb02e6e94c23d2bfca59c4d0836264ea892efbb65b6d9f8") },
                {  58000, uint256S("0x97235c555f8d072236da56807bfe0f8ca32edfca5fc864b4b233045338bcf0bc") },
                {  59000, uint256S("0x64305c04fe6883c3eeefe85a03c9b7b7f223d7310480b69f54908e417dc3c20f") },
                {  60000, uint256S("0x1128e28907b17c292aaa32f3c3d9e06650fb0436683797cd98731a688e408866") },
                {  61000, uint256S("0x765a58f5ec90d4c53b01781b1bcaffeac5aa7492047521a250c1d87c5f8fb858") },
                {  62000, uint256S("0xcb32e25b1586aefc293044ea9d9fdd08d8ce36608cec29bc7a4022a5e73e2cb8") },
                {  63000, uint256S("0x05f6fa5db2beeed859e768a50d020a16611380fcfe3735dc3373e008a181c283") },
                {  64000, uint256S("0xf39de31b4f07eab4202f922e65d0722f2f6c5f12d89fe638fa64297d2c29a63f") },
                {  65000, uint256S("0x34b9817113b21963a78861dcdbaa03e34eee21b61ace49d8092c52c2f5b06a51") },
                {  66000, uint256S("0xe2f7a9bf0b5ad6630c8623448bd1e6669a52107c74edfc1e33c7cf22030b5cdc") },
                {  67000, uint256S("0x382e79f0ca05e6515eb7afdd7500adef354b2774910eea657a583f445fdbeac4") },
                {  68000, uint256S("0xd4f8fb21c8079683d76efaaa79405f11eb9948e9ea6578dabcbe70a3ed4e0673") },
                {  69000, uint256S("0x5560a6c2ff84094a9f0fe242b27750212254235557525a76872958f39eb81161") },
                {  70000, uint256S("0x7c0d0abf50bdab5960a6f80d966ad76b6c5fd9098ae66fc09c683deba8ac6ccd") },
                {  71000, uint256S("0x1e4025e4fba50e3aeaa020ddc409398c2bb1a4ea03cd738bfc79a8674132bdb0") },
                {  72000, uint256S("0x93065a3b4e43baf4b6dbba22c5cd560ce9c9c04310ed81b9519ef75d2d37cc4e") },
                {  73000, uint256S("0xfcc5e8d7bf51d262bb501543c430b30c00915f4ba0e9e572e24a5ef858f6684b") },
                {  74000, uint256S("0x52578073a170ba035f5a1d7dc4906cf57dc4aa8392cb64967f8943455fabdbf4") },
                {  75000, uint256S("0x1c0104c50e11591420eb5bde8aeeaf2149bcb9351bfb5bac658f87b8ec53b4ed") },
                {  76000, uint256S("0xc6163360b107368ebc89556acb2c27235e210dc11809b92dfb03120f65fdcc99") },
                {  77000, uint256S("0xc6b2a4c3680909bb80d0f3728f56fecbca726739098a8071adc002a4fe5323f2") },
                {  78000, uint256S("0xb089b8275035b1e07efe568bc33e7169738e50fba59393840aafaa73f4c708e8") },
                {  79000, uint256S("0xd8966c3db28ef38eaeaf14fa99ccc90a193afd6bb6c0f9c8a60c6d0ef5ab477d") },
                {  80000, uint256S("0x2a099ed535d307ce75ac18638b8bf7dd41cf5624e7b66bda4c100c03caca80cb") },
                {  81000, uint256S("0xce0d6dd75f8181a63df5867cfc0e62bca05de1b6d5e81c26bdf6f146c6305bd8") },
                {  82000, uint256S("0x2a3d140578f0edc3905372747311da8827f459f42d68a02fcd8cd9f1bce75978") },
                {  83000, uint256S("0x12a88b8a899e0711324d9708fd3aad49c2afbd3d2f26e388a42eefa8a9d56c2a") },
                {  84000, uint256S("0xe024d36300682e760016801d3497d5cbdf727917bd37e0d1925663978e6126ee") },
                {  85000, uint256S("0x12389594cc1a13b1eaad1d5147351f6ff6b521580d19494b10488e47853c9d9c") },
                {  86000, uint256S("0x25fbb338888fd516b38b73c983477309e03f5ec5ce8b46dff476c12e878b490e") },
                {  87000, uint256S("0x0d2fda2ec88033e28f7e37b5db95e57708a059f5aa9bc62d754415f1c1018e33") },
                {  88000, uint256S("0x6865a5ccc845095cfdb28a706d209597b5d78f23446a5b02805dc271e205c70e") },
                {  89000, uint256S("0xb12ae2f47ac0ade9341676845c290b3f7034d42321c528de959de424e751b47c") },
                {  90000, uint256S("0x827a252f93f32c8def8a5ba64cc9582c8fc199bd711ee85112aef53988ce3b2b") },
                {  91000, uint256S("0x8cb205c3bf7b39126ffaced2eb5aabdaa6c0c6d74a29cec8a42aa307bc4cf524") },
                {  92000, uint256S("0x200146acaf2286c1002b9ba9bb23ff9765ce2f6caf5b544b39f984144e8a9cb4") },
                {  93000, uint256S("0xd60352b72477538a59d563cdaf27114dd08be3b4aed1eef18253f92d5f971d2b") },
                {  94000, uint256S("0x014d5732756a32fff1fc6dccb0fd53592b7e2337ca6072fc6316687ab511c4ab") },
                {  95000, uint256S("0xe6261f48218ead2880c417e11964742847ef2431625dccd41a81bd8990de4a32") },
                {  96000, uint256S("0x466ff7bf63a9d47c5f716e39eb525e72f8d5f8d1d3b1c55d6a2c56097c0bc105") },
                {  97000, uint256S("0x8ab3bdd033df8ee77f90cb1917802f0edce7090532fc6a889395584e31f4115b") },
                {  98000, uint256S("0x7534314e5ceba6ba39c2c9244dec8816d801d0d0eabbe956ec9c952be5720156") },
                {  99000, uint256S("0xcbd672321af1cf138c24d052f81b09466c44515b058287b23de6c7ccb68f9651") },
                {  100000, uint256S("0x94e80b2871db28950e9005890a5b5ee875cf453947978462f46b705ce80066d8") },
                {  101000, uint256S("0xb614579020d81a5882f3a36ff0898cbbcbfd1d63c4f1b6f835add2a03767905c") },
                {  102000, uint256S("0x6aa5cbf5e16cbe9e5b44ccac67c9394a49edfc9bcbb3b18974ad093fdf4b5bc6") },
                {  103000, uint256S("0xe33f5f3479699b77a1bbc70c586f539389d23163bfd9a7e5d544311e82a1ca52") },
                {  104000, uint256S("0x44905a3381d8bf3e15daa5258cd9332619bd69670d6df7e94701c4e748cc5de1") },
                {  105000, uint256S("0x3d414ad47acf9aed9e26cf681298d0710d4fb68d4057e16d765cd6e26e3fbee3") },
                {  106000, uint256S("0x074b174cdffb56727d71f08f2002957d89b3ff39c87abb69a5e990c462a81c8b") },
                {  106848, uint256S("0xb3482ee2d4fde29559636657de6f1059b4f8b38ba2d8c9842078d4faed53d92e") }, // xdiskIP006
                {  107000, uint256S("0x142123211cf8df7011baa5d3f4f485cd6484920124b4ecd135e72e2f7b99b2ed") },
                {  108000, uint256S("0x8e03e9959f26dad803ce5ef10ad8f974854b0f663ac699fcc38fa7999f10d072") },
                {  109000, uint256S("0x47dbf344dfc9731c56dd4ee00d4c9d9c7125018d1046c6e81ead8ecd6ba0829d") },
                {  109500, uint256S("0x87facc4009fa5eef8d7d50168eae18ca0e8a9ce9a0c5d41fb828c2058553c497") },
            }
        };

        chainTxData = ChainTxData{
            1565880684,
            111173,
            0.003756
        };
    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";

        consensus.xdiskFundAddress = "2NDHUkujmJ3SBL5JmFZrycxGbAumhr2ycgy"; // pubkey 03eab29d59f6d14053c6e98f6d3d7e7db9cc17c619a513b9c00aa416fbdada73f1
        consensus.xdiskFundAddressPool = { "2NDHUkujmJ3SBL5JmFZrycxGbAumhr2ycgy" };
        assert(consensus.xdiskFundAddressPool.find(consensus.xdiskFundAddress) != consensus.xdiskFundAddressPool.end());

        consensus.nCapacityEvalWindow            = 144;
        consensus.nSubsidyHalvingInterval        = 300;
        consensus.fAllowMinDifficultyBlocks      = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow       = 144;

        consensus.xdiskIP001PreMiningEndHeight         = 84; // 21M * 0.01% = 0.0021M, 0.0021M/25=84
        consensus.xdiskIP001FundZeroLastHeight         = 94;
        consensus.xdiskIP001TargetSpacing              = 300;
        consensus.xdiskIP001FundRoyaltyForFullMortgage = 50; // 50‰
        consensus.xdiskIP001FundRoyaltyForLowMortgage  = 700; // 700‰
        consensus.xdiskIP001MiningRatio                = 3 * COIN;

        consensus.xdiskIP004Height        = 0;
        consensus.xdiskIP004AbandonHeight = 0;

        consensus.xdiskIP006Height                  = 294;
        consensus.xdiskIP006BindPlotterActiveHeight = 344;
        consensus.xdiskIP006CheckRelayHeight        = 488;
        consensus.xdiskIP006LimitBindPlotterHeight  = 493;

        consensus.xdiskIP007Height           = 550;
        consensus.xdiskIP007SmoothEndHeight  = 586;
        consensus.xdiskIP007MiningRatioStage = 10 * 1024; // 10 PB

        consensus.xdiskIP008Height                                  = 720;
        consensus.xdiskIP008TargetSpacing                           = 180;
        consensus.xdiskIP008FundRoyaltyForLowMortgage               = 270;
        consensus.xdiskIP008FundRoyaltyDecreaseForLowMortgage       = 20;
        consensus.xdiskIP008FundRoyaltyDecreasePeriodForLowMortgage = 36;
        assert(consensus.xdiskIP008Height % consensus.nMinerConfirmationWindow == 0);
        assert(consensus.xdiskIP008FundRoyaltyForLowMortgage < consensus.xdiskIP001FundRoyaltyForLowMortgage);
        assert(consensus.xdiskIP008FundRoyaltyForLowMortgage > consensus.xdiskIP001FundRoyaltyForFullMortgage);

        consensus.BIP16Height = 0; // Always enforce BIP16
        consensus.BIP34Height = 0; // Always enforce BIP34
        consensus.BIP65Height = 0; // Always enforce BIP65
        consensus.BIP66Height = 0; // Always enforce BIP66

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xe6;
        pchMessageStart[1] = 0xbb;
        pchMessageStart[2] = 0xb1;
        pchMessageStart[3] = 0xd6;
        nDefaultPort = 18744;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1531292789, 2, poc::GetBaseTarget(240), 2, 50 * COIN * consensus.xdiskIP001TargetSpacing / 600);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x8414542ce030252cd4958545e6043b8c4e48182756fe39325851af58922b7df6"));
        assert(genesis.hashMerkleRoot == uint256S("0xb17eff00d4b76e03a07e98f256850a13cd42c3246dc6927be56db838b171d79b"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;

        checkpointData = {
            {
                {0, uint256S("0x8414542ce030252cd4958545e6043b8c4e48182756fe39325851af58922b7df6")},
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "bcrt";
    }
};

static std::unique_ptr<CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams());
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}

void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    globalChainParams->UpdateVersionBitsParameters(d, nStartTime, nTimeout);
}
