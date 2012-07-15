;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  Jmp
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
JumpInstruction@0 proc public
else
JumpInstruction proc public
endif
ifdef X64
	mov rax, 0AABBCCDDEEFF0011h
	jmp rax
else
	mov eax, 0AABBCCDDh
	jmp eax
endif
	int 3; Nice end signature.
	int 3
	int 3
	int 3
ifndef X64
JumpInstruction@0 endp
else
JumpInstruction endp
endif

end