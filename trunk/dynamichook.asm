;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  Dynamic Detour/Hooking function
;;  
;;  Copyright (c) 2010-2012 Harry Pidcock
;;  
;;  Permission is hereby granted, free of charge, to any person
;;  obtaining a copy of this software and associated documentation
;;  files (the "Software"), to deal in the Software without
;;  restriction, including without limitation the rights to use,
;;  copy, modify, merge, publish, distribute, sublicense, and;or sell
;;  copies of the Software, and to permit persons to whom the
;;  Software is furnished to do so, subject to the following
;;  conditions:
;;  
;;  The above copyright notice and this permission notice shall be
;;  included in all copies or substantial portions of the Software.
;;  
;;  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
;;  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
;;  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
;;  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
;;  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
;;  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
;;  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
;;  OTHER DEALINGS IN THE SOFTWARE.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ifndef X64
.model flat, c
endif

.code

ifndef X64
DynamicHook@0 proc public
else
DynamicHook proc public
endif
ifdef X64

	;
	; Begin x64
	;

	push rbp
	mov rbp, rsp
	sub rsp, 90h; Stick to 16byte boundaries
	push rbx
	push rsi
	push rdi

	; 8 bytes
	mov rbx, 0AABBCCDDh
	mov [rbp-8], rbx

	; 32 bytes
	; Arg 1 - rcx/xmm0
	mov [rbp-10h], rcx
	movsd qword ptr [rbp-18h], xmm0
	; Arg 2 - rdx/xmm1
	mov [rbp-20h], rdx
	movsd qword ptr [rbp-28h], xmm1
	; Arg 3 - r8/xmm2
	mov [rbp-30h], r8
	movsd qword ptr [rbp-38h], xmm2
	; Arg 4 - r9/xmm3
	mov [rbp-40h], r9
	movsd qword ptr [rbp-48h], xmm3

	; 32 bytes for scratch?
	mov r10, [rbx] ; m_pDetour
	mov [rbp-50h], r10
	
	mov r10, [rbx+8] ; m_pOriginalEntry
	mov [rbp-58h], r10

	mov r10, [rbx+10h] ; m_iArgCount
	mov [rbp-60h], r10

	mov r10, [rbx+18h]
	mov [rbp-70h], r10 ; First Pre Hook

	mov r10, [rbx+20h]
	mov [rbp-78h], r10 ; Pre Hook Count

	mov r10, [rbx+28h]
	mov [rbp-80h], r10 ; First Post Hook

	mov r10, [rbx+30h]
	mov [rbp-88h], r10 ; Post Hook Count


	; Call the pre hooks
	sub rsp, 20h ; Shadow Space
	xor rsi, rsi
	mov rdi, [rbp-78h]
	test rdi, rdi
	je hookEndLoop
	mov rdi, rdi
hookLoop:
	mov r11, [rbp-70h]
	mov r11, [r11+rsi*8]

	mov rbx, rsp
	mov r10, [rbp-60h]
	cmp r10, 4
	jle endLoop
	sub r10, 4; First 4 arguments handled above
	test r10, r10
	jz endLoop
argLoop:
	mov rax, [rbp + r10 * 8 + 10h]
	push rax
	test r10, r10
	jz endLoop
	sub r10, 1
	jmp argLoop
endLoop:
	call r11
	mov rsp, rbx

	inc rsi
	cmp rsi, rdi
	jb hookLoop
hookEndLoop:

	; Restore Register Args
	; Arg 1 - rcx/xmm0
	mov rcx, [rbp-10h]
	movsd xmm0, qword ptr [rbp-18h]
	; Arg 2 - rdx/xmm1
	mov rdx, [rbp-20h]
	movsd xmm1, qword ptr [rbp-28h]
	; Arg 3 - r8/xmm2
	mov r8, [rbp-30h]
	movsd xmm2, qword ptr [rbp-38h]
	; Arg 4 - r9/xmm3
	mov r9, [rbp-40h]
	movsd xmm3, qword ptr [rbp-48h]

	; Call Detour or Original Function
	; Choose the detour or the original function
	mov rax, [rbp-50h]
	mov [rbp-68h], rax
	test rax, rax
	jne useDetour
	mov rax, [rbp-58h]
	mov [rbp-68h], rax
useDetour:

; Call the function
	mov rbx, rsp
	mov r10, [rbp-60h]
	cmp r10, 4
	jle endLoopb
	sub r10, 4; First 4 arguments handled above
	test r10, r10
	jz endLoopb
argLoopb:
	mov rax, [rbp + r10 * 8 + 10h]
	push rax
	test r10, r10
	jz endLoopb
	sub r10, 1
	jmp argLoopb
endLoopb:
	mov rax, [rbp-68h]
	call rax
	mov rsp, rbx

	add rsp, 20h ; Shadow Space

	; Save Return
	; rax, xmm0, ymm0, st(0)
	push rax
	sub rsp, 10h
	movupd [rsp], xmm0

	; Restore Register Args
	; Arg 1 - rcx/xmm0
	mov rcx, [rbp-10h]
	movsd xmm0, qword ptr [rbp-18h]
	; Arg 2 - rdx/xmm1
	mov rdx, [rbp-20h]
	movsd xmm1, qword ptr [rbp-28h]
	; Arg 3 - r8/xmm2
	mov r8, [rbp-30h]
	movsd xmm2, qword ptr [rbp-38h]
	; Arg 4 - r9/xmm3
	mov r9, [rbp-40h]
	movsd xmm3, qword ptr [rbp-48h]

	; Call Post Hooks
	sub rsp, 20h ; Shadow Space
	xor rsi, rsi
	mov rdi, [rbp-88h]
	test rdi, rdi
	je hookEndLoopc
	mov rdi, rdi
hookLoopc:
	mov r11, [rbp-80h]
	mov r11, [r11+rsi*8]

	mov rbx, rsp
	mov r10, [rbp-60h]
	cmp r10, 4
	jle endLoopc
	sub r10, 4; First 4 arguments handled above
	test r10, r10
	jz endLoopc
argLoopc:
	mov rax, [rbp + r10 * 8 + 10h]
	push rax
	test r10, r10
	jz endLoopc
	sub r10, 1
	jmp argLoopc
endLoopc:
	call r11
	mov rsp, rbx

	inc rsi
	cmp rsi, rdi
	jb hookLoopc
hookEndLoopc:
	add rsp, 20h ; Shadow Space


	; Restour rax, ymm0
	movupd xmm0, [rsp]
	add rsp, 10h
	pop rax


	; Restore
	pop rdi
	pop rsi
	pop rbx
	add rsp, 90h
	pop rbp
	ret

	;
	; End x64
	;

else

	;
	; Begin x86
	;

	push ebp
	mov ebp, esp
	sub esp, 34h
	push ebx
	push esi
	push edi
	mov [ebp-0Ch], ecx
	mov ecx, 0AABBCCDDh
	mov [ebp-4], ecx

	mov edi, [ecx+0Ch]
	mov [ebp-28h], edi ; First Pre Hook

	mov edi, [ecx+10h]
	mov [ebp-2Ch], edi ; Pre Hook Count

	mov edi, [ecx+14h]
	mov [ebp-30h], edi ; First Post Hook

	mov edi, [ecx+18h]
	mov [ebp-34h], edi ; Post Hook Count

; Call the pre hooks
	xor esi, esi
	mov edi, [ebp-2Ch]
	mov edx, [ecx+8]
	mov [ebp-14h], edx
	mov [ebp-8], edx
	test edi, edi
	je hookEndLoop
	mov edi, edi
hookLoop:
	mov eax, [ebp-28h]
	mov eax, [eax+esi*4]
	mov [ebp-10h], eax
	mov ebx, esp
	mov ecx, [ebp-8]
	jcxz endLoop
argLoop:
	mov eax, [ebp+ecx*4+8]
	push eax
	jcxz endLoop
	sub ecx, 1
	jmp argLoop
endLoop:
	mov ecx, [ebp-0Ch]
	mov eax, [ebp-10h]
	call eax
	mov esp, ebx
	mov ecx, [ebp-4]
	inc esi
	cmp esi, edi
	jb hookLoop
	mov edx, [ebp-14h]
hookEndLoop:


; Choose the detour or the original function
	mov eax, [ecx]
	mov [ebp-10h], eax
	test eax, eax
	jne useDetour
	mov eax, [ecx+4]
	mov [ebp-10h], eax
useDetour:

; Call the function
	lea eax, [edx*4]
	mov [ebp-24h], eax
	mov ebx, esp
	mov ecx, [ebp-8]
	jcxz endLoopb
argLoopb:
	mov eax, [ebp+ecx*4+8]
	push eax
	jcxz endLoopb
	sub ecx, 1
	jmp argLoopb
endLoopb:
	mov ecx, [ebp-0Ch]
	mov eax, [ebp-10h]
	call eax
	mov esp, ebx

; Save our return values
	sub esp, 8
	fstp qword ptr [esp]
	push eax
	push edx

; Call the return hooks
	xor esi, esi
	mov edi, [ebp-34h]
	test edi, edi
	je endRetHooksLoop
retHooksLoop:
	mov eax, [ebp-30h]
	mov eax, [eax+esi*4]
	mov [ebp-14h], eax
	mov ebx, esp
	mov ecx, [ebp-8]
	jcxz endLoop2
argLoop2:
	mov eax, [ebp+ecx*4+8]
	push eax
	jcxz endLoop2
	sub ecx, 1
	jmp argLoop2
endLoop2:
	mov ecx, [ebp-0Ch]
	mov eax, [ebp-14h]
	call eax
	mov esp, ebx
	inc esi
	cmp esi, edi
	jae endRetHooksLoop
	mov eax, [ebp-4]
	jmp retHooksLoop
endRetHooksLoop:


	pop edx
	pop eax
	fld qword ptr [esp]
	add esp, 8
	movss xmm1, dword ptr [ebp-24h]
	pop edi
	pop esi
	pop ebx
	mov esp, ebp
	pop ebp
	movss xmm3, dword ptr [esp]
	mov [esp], esp
	movss xmm2, dword ptr [esp]
	paddd xmm2, xmm1
	movss dword ptr [esp], xmm2
	mov esp, [esp]
	movss dword ptr [esp], xmm3
	ret

	;
	; End x86
	;

endif
	int 3; Nice end signature.
	int 3
	int 3
	int 3
ifndef X64
DynamicHook@0 endp
else
DynamicHook endp
endif

end