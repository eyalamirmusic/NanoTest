#include <NanoTest/NanoTest.h>
#include <NanoTest/Registry.h>

#include <source_location>
#include <string>
#include <vector>

using namespace nano;

namespace
{
const auto caseNames = std::vector<std::string> {
    "Alpha", "Alpha/one", "Alpha/sub/two", "AlphaBeta/one", "Beta/one"};

struct RecordingRegistry
{
    RecordingRegistry()
    {
        for (auto& name: caseNames)
            registry.add(name, [this, name] { ran.push_back(name); });
    }

    std::vector<std::string> runWith(const RunOptions& options)
    {
        ran.clear();
        exitCode = registry.run(options);
        return ran;
    }

    Registry registry;
    std::vector<std::string> ran;
    int exitCode = -1;
};

using Names = std::vector<std::string>;
} // namespace

auto noFilterRunsEverything = test("Filter/noFilterRunsEverything") = []
{
    auto recorder = RecordingRegistry();
    check(recorder.runWith({}) == caseNames);
    check(recorder.exitCode == 0);
};

auto exactTestRunsOneCase = test("Filter/exactTestRunsOneCase") = []
{
    auto recorder = RecordingRegistry();
    check(recorder.runWith({.test = "Alpha/one"}) == Names {"Alpha/one"});
    check(recorder.runWith({.test = "Alpha"}) == Names {"Alpha"});
    check(recorder.runWith({.test = "Alph"}).empty());
};

auto groupRunsTheCaseAndEverythingUnderIt =
    test("Filter/groupRunsTheCaseAndEverythingUnderIt") = []
{
    auto recorder = RecordingRegistry();
    auto expected = Names {"Alpha", "Alpha/one", "Alpha/sub/two"};
    check(recorder.runWith({.group = "Alpha"}) == expected);
    check(recorder.exitCode == 0);
};

auto groupStopsAtASegmentBoundary = test("Filter/groupStopsAtASegmentBoundary") = []
{
    auto recorder = RecordingRegistry();
    check(recorder.runWith({.group = "Alph"}).empty());
    check(recorder.runWith({.group = "AlphaBeta"}) == Names {"AlphaBeta/one"});
};

auto groupAcceptsATrailingSlash = test("Filter/groupAcceptsATrailingSlash") = []
{
    auto recorder = RecordingRegistry();
    auto expected = Names {"Alpha", "Alpha/one", "Alpha/sub/two"};
    check(recorder.runWith({.group = "Alpha/"}) == expected);
};

auto groupCanBeDeeper = test("Filter/groupCanBeDeeper") = []
{
    auto recorder = RecordingRegistry();
    check(recorder.runWith({.group = "Alpha/sub"}) == Names {"Alpha/sub/two"});
};

auto testAndGroupCombine = test("Filter/testAndGroupCombine") = []
{
    auto recorder = RecordingRegistry();
    auto expected = Names {"AlphaBeta/one", "Beta/one"};
    check(recorder.runWith({.test = "AlphaBeta/one", .group = "Beta"}) == expected);
};

auto isInGroupMatchesSegments = test("Filter/isInGroupMatchesSegments") = []
{
    check(Registry::isInGroup("Net", "Net"));
    check(Registry::isInGroup("Net/get", "Net"));
    check(Registry::isInGroup("Net/Socket/open", "Net/Socket"));
    check(!Registry::isInGroup("Network/get", "Net"));
    check(!Registry::isInGroup("Ne", "Net"));
};

auto failingCaseDoesNotStopTheGroup =
    test("Failures/failingCaseDoesNotStopTheGroup") = []
{
    auto registry = Registry();
    auto ran = std::vector<std::string>();

    auto pass = [&ran](const std::string& name)
    { return [&ran, name] { ran.push_back(name); }; };

    registry.add("Group/before", pass("Group/before"));
    registry.add("Group/fails",
                 [&]
                 {
                     ran.push_back("Group/fails");
                     registry.fail(std::source_location::current(), "false");
                 });
    registry.add("Group/after", pass("Group/after"));

    auto exitCode = registry.run({.group = "Group"});

    check(exitCode == 1);
    check(ran == Names {"Group/before", "Group/fails", "Group/after"});
};

auto failureOnlyAffectsItsOwnCase =
    test("Failures/failureOnlyAffectsItsOwnCase") = []
{
    auto registry = Registry();
    registry.add("Group/fails",
                 [&] { registry.fail(std::source_location::current(), "x"); });
    registry.add("Group/passes", [] {});

    check(registry.run({.test = "Group/passes"}) == 0);
    check(registry.run({.test = "Group/fails"}) == 1);
};
