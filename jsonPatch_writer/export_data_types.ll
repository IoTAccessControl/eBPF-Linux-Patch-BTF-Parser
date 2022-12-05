; ModuleID = 'export_data_types.c'
source_filename = "export_data_types.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.Stack_frame = type { i64, i64, i64, i64, i64, i64, i64 }
%struct.h264Picture = type { i32 }
%struct.h264Context = type { %struct.h264Picture*, [18 x %struct.h264Picture*] }

@Stack_frame = global %struct.Stack_frame* null, align 8
@h264Picture = global %struct.h264Picture* null, align 8
@h264Context = global %struct.h264Context* null, align 8

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 14.0.0-1ubuntu1"}
