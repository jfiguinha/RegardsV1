;******************************************************************************
;
; Copyright (c) 1999 Advanced Micro Devices, Inc.
;
; LIMITATION OF LIABILITY:  THE MATERIALS ARE PROVIDED *AS IS* WITHOUT ANY
; EXPRESS OR IMPLIED WARRANTY OF ANY KIND INCLUDING WARRANTIES OF MERCHANTABILITY,
; NONINFRINGEMENT OF THIRD-PARTY INTELLECTUAL PROPERTY, OR FITNESS FOR ANY
; PARTICULAR PURPOSE.  IN NO EVENT SHALL AMD OR ITS SUPPLIERS BE LIABLE FOR ANY
; DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS,
; BUSINESS INTERRUPTION, LOSS OF INFORMATION) ARISING OUT OF THE USE OF OR
; INABILITY TO USE THE MATERIALS, EVEN IF AMD HAS BEEN ADVISED OF THE POSSIBILITY
; OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE EXCLUSION OR LIMITATION
; OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES, THE ABOVE LIMITATION MAY
; NOT APPLY TO YOU.
;
; AMD does not assume any responsibility for any errors which may appear in the
; Materials nor any responsibility to support or update the Materials.  AMD retains
; the right to make changes to its test specifications at any time, without notice.
;
; NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
; further information, software, technical information, know-how, or show-how
; available to you.
;
; So that all may benefit from your experience, please report  any  problems
; or  suggestions about this software to 3dsdk.support@amd.com
;
; AMD Developer Technologies, M/S 585
; Advanced Micro Devices, Inc.
; 5900 E. Ben White Blvd.
; Austin, TX 78741
; 3dsdk.support@amd.com
;
;******************************************************************************
;
; AMDK7.INC
;
; AMD K7 macros, assumes MASM 6.14
;
;******************************************************************************

amd3dop  macro   op:req, dst:req, src:req
if ((OPATTR(dst)) AND 00010000y) ; register mode
    pfadd dst, src
    org $-1
    db  op
else
    .err <*** Error: only register allowed as destination>
endif
    endm

;-------------------------------------------------------------------
pfnacc  macro   dst:req, src:req
        amd3dop         08Ah, dst, src
        endm

pfpnacc macro   dst:req, src:req
        amd3dop         08Eh, dst, src
        endm

pswapd  macro   dst:req, src:req
        amd3dop         0BBh, dst, src
        endm


; MASM thinks a "syscall" is something other than an opcode, so
; this must have a unique name
amd_syscall macro
    db  0fh, 05h
    endm

sysret  macro
    db  0fh, 07h
    endm

sysenter    macro
    db  0fh, 34h
    endm

sysexit macro
    db  0fh, 35h
    endm


ffreep  macro   dst:req
MMX_OPCODE = $
    ffree   dst
MMX_NEXT = $
    org MMX_OPCODE
    db  0DFH
    org MMX_NEXT
    endm

; eof
