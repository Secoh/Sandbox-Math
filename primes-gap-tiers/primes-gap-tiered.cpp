// MIT license

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include <sklib/include/cmdpar.hpp>
//#include <sklib/include/math.hpp>

SKLIB_DECLARE_CMD_PARAMS(options_t)
{
    SKLIB_PARAM_STRING(hist);  // prime indexing gaps histogram, created by generator
    SKLIB_PARAM_INT(Lmax);     // maximum bit length to encode gap size (will be verified)
    SKLIB_PARAM_INT(N);        // count of tiers to consider
    SKLIB_OPTION_INT(v, 0);    // verbose w/ level
}
Options;

static constexpr int verbose_none = -1;
static constexpr int verbose_basic = 0;
static constexpr int verbose_data = 1;
static constexpr int verbose_detail = 2;

// for the given tiers distribution, compute the total encoding length
size_t try_tiers(const std::vector<size_t>& hist, const std::vector<int>& tiers)
{
    int k = -1;
    int T = 0;
    int L = 0;
    int B = 0;
    int W = 0;
    size_t S = 0;

    static constexpr int Tmax = sklib::OCTET_BITS;

    auto Nhist = hist.size();
    auto Ntiers = tiers.size();

    if (Options.v.is_present())
    {
        for (size_t k=0; k<Ntiers; k++) std::cout << tiers[k] << " ";
        std::cout << "\n";
    }

    for (size_t i=0; i<Nhist; i++)
    {
        if (i >= B)  // tier step
        {
            k++;
            T = (k >= (int)Ntiers ? Tmax : tiers[k]);
            L = (1 << T) - 1;
            B = B + L;
            W = W + T;
        }

        S = S + W * hist[i];

        if (Options.v.value() >= verbose_detail)
        {
            std::cout << "i=" << i << ", k=" << k << ", T=" << T << ", L=" << L
                      << ", B=" << B << ", W=" << W << ", H=" << hist[i]
                      << ", S=" << S << "\n";
        }
    }

    if (Options.v.is_present()) std::cout << "Ret=" << S << "\n";

    return S;
}

int main(int argn, char* argc[])
{
    if (!Options(argn, argc))
    {
        std::cout << "Given the integer descending 0-based histogram (max 255 position),\n"
                     "calculate the packed bit size of the archive by simplified LZ algorithm.\n"
                     "Usage: primes-gap-tiered.exe -hist primes_generator_output.csv\n"
                     "                       -Lmax maximum bit length of one tier (no more than 8)\n"
                     "                       -N count of tiers to consider [-v verbose level]\n";
        return 1;
    }

    int verbose_level = (Options.v.is_present() ? Options.v.value() : verbose_none);

    if (Options.N < 1)
    {
        std::cout << "There must be at least 1 tier\n";
        return -1;
    }

    std::ifstream fhist(Options.hist);
    if (!fhist.is_open())
    {
        std::cout << "Cannot read file " << Options.hist << "\n";
        return -1;
    }

    std::vector<size_t> hist;

    std::string input;
    for (int i = 0; i < sklib::OCTET_ADDRESS_SPAN && std::getline(fhist, input); i++)
    {
        std::string ignore, sord, sdata;
        std::stringstream istr{input};

        std::getline(istr, ignore, ',');
        std::getline(istr, ignore, ',');
        std::getline(istr, sord, ',');
        std::getline(istr, sdata, ',');

        auto ord = sklib::stoi<int>(sord.c_str());
        auto data = sklib::stoi<size_t>(sdata.c_str());

        if (i != ord)
        {
            std::cout << "Bad structure of histogram file " << Options.hist << "\n";
            return -1;
        }

        hist.push_back(data);

        if (Options.v.value() >= verbose_data) std::cout << i << ", " << data << "\n";
    }

    if (Options.v.is_present()) std::cout << "Read " << hist.size() << " histogram records\n";

    // test
    // std::cout << try_tiers(hist, {2,3,7}) << "\n";

    std::vector<int> tiers;
    for (size_t k=0; k<Options.N; k++) tiers.push_back(1);

    size_t Smin = 0;
    std::vector<int> tiers_min;

    bool keep_counting = true;
    while (keep_counting)
    {
        auto S = try_tiers(hist, tiers);
        if (!Smin || S < Smin)
        {
            Smin = S;
            tiers_min = tiers;
        }

        size_t kk = 0;
        while (++tiers[kk] >= Options.Lmax)
        {
            tiers[kk++] = 1;
            if (kk >= tiers.size())
            {
                keep_counting = false;
                break;
            }
        }
    }

    static constexpr size_t KB = 1024;
    static constexpr size_t MB = KB*KB;

    size_t Sb = (Smin + sklib::OCTET_BITS - 1) / sklib::OCTET_BITS;
    size_t Skb = (Sb + KB - 1) / KB;
    size_t Smb = (Sb + MB - 1) / MB;

    for (size_t k=0; k<tiers_min.size(); k++) std::cout << tiers_min[k] << " ";
    std::cout << "\nBit count: " << Smin << "\n"
              << "Bytes:" << Sb << ", kilobytes: " << Skb << ", Megabytes: " << Smb << "\n";

    return 0;
}

