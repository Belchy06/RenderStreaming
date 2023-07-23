// Copyright Epic Games, Inc. All Rights Reserved.

using System.Collections.Generic;
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class RenderStreaming : ModuleRules
    {
        private List<string> GrpcLibs = new List<string>
        {
            "grpc", "grpc++", "gpr", "upb", "address_sorting"
        };
        private List<string> ProtobufLibs = new List<string>
        {
            "libprotobuf"
        };
        private List<string> AbseilLibs = new List<string>
        {
            "absl_hash", "absl_city", "absl_wyhash", "absl_raw_hash_set", "absl_hashtablez_sampler", "absl_exponential_biased", "absl_statusor",
            "absl_bad_variant_access", "absl_status", "absl_cord", "absl_str_format_internal", "absl_synchronization", "absl_stacktrace",
            "absl_symbolize", "absl_debugging_internal", "absl_demangle_internal", "absl_graphcycles_internal", "absl_malloc_internal", "absl_time",
            "absl_strings", "absl_throw_delegate", "absl_strings_internal", "absl_base", "absl_spinlock_wait", "absl_int128", "absl_civil_time",
            "absl_time_zone", "absl_bad_optional_access", "absl_raw_logging_internal", "absl_log_severity",
        };
        private List<string> Re2Libs = new List<string>
        {
            "re2"
        };

        public RenderStreaming(ReadOnlyTargetRules Target) : base(Target)
        {
            // This is so for game projects using our public headers don't have to include extra modules they might not know about.
            PublicDependencyModuleNames.AddRange(new string[]
            {
            });

            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "Core",
                "Engine",
                "PixelStreaming",
                "WebRTC",
                "Json"
            });

            string ThirdPartyPath = "F:/Work/Git/UnrealEngine/Engine/Source/ThirdParty";

            //ThirdParty include
            PrivateIncludePaths.AddRange(new string[] {
                Path.Combine(ThirdPartyPath, "protobuf/include"),
                Path.Combine(ThirdPartyPath, "grpc/include"),
                Path.Combine(ThirdPartyPath, "abseil/include")
            });

            foreach (string lib in GrpcLibs)
            {
                string fullPath = ThirdPartyPath + "/grpc/" + "lib/win64/Release/" + lib + ".lib";
                PublicAdditionalLibraries.Add(fullPath);
            }

            foreach (string lib in ProtobufLibs)
            {
                string fullPath = ThirdPartyPath + "/protobuf/" + "lib/win64/Release/" + lib + ".lib";
                PublicAdditionalLibraries.Add(fullPath);
            }

            foreach (string lib in AbseilLibs)
            {
                string fullPath = ThirdPartyPath + "/abseil/" + "lib/win64/Release/" + lib + ".lib";
                PublicAdditionalLibraries.Add(fullPath);
            }

            foreach (string lib in Re2Libs)
            {
                string fullPath = ThirdPartyPath + "/re2/" + "lib/win64/Release/" + lib + ".lib";
                PublicAdditionalLibraries.Add(fullPath);
            }

            AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenSSL");
            AddEngineThirdPartyPrivateStaticDependencies(Target, "zlib");

            PublicDefinitions.Add("GOOGLE_PROTOBUF_INTERNAL_DONATE_STEAL_INLINE=0");
        }
    }
}
