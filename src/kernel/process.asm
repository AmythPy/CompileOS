; CompileOS Process Context Switching
; Low-level assembly for switching between processes

[BITS 32]

global context_switch

; void context_switch(cpu_context_t* old_context, cpu_context_t* new_context)
; Save current context and restore new context
context_switch:
    ; Get parameters
    mov eax, [esp + 4]      ; old_context
    mov edx, [esp + 8]      ; new_context
    
    ; Save old context
    mov [eax + 0],  ebx     ; Save ebx (eax is used for pointer)
    mov [eax + 8],  ecx     ; Save ecx
    mov [eax + 12], edx     ; Save edx (will be overwritten)
    mov [eax + 16], esi     ; Save esi
    mov [eax + 20], edi     ; Save edi
    mov [eax + 24], ebp     ; Save ebp
    mov [eax + 28], esp     ; Save esp
    
    ; Save eip (return address)
    mov ebx, [esp]
    mov [eax + 32], ebx     ; Save eip
    
    ; Save eflags
    pushfd
    pop ebx
    mov [eax + 36], ebx     ; Save eflags
    
    ; Save segment registers
    mov bx, cs
    mov [eax + 40], bx      ; Save cs
    mov bx, ds
    mov [eax + 42], bx      ; Save ds
    mov bx, es
    mov [eax + 44], bx      ; Save es
    mov bx, fs
    mov [eax + 46], bx      ; Save fs
    mov bx, gs
    mov [eax + 48], bx      ; Save gs
    mov bx, ss
    mov [eax + 50], bx      ; Save ss
    
    ; Restore new context
    mov eax, edx            ; eax = new_context
    
    ; Restore segment registers
    mov bx, [eax + 42]      ; Restore ds
    mov ds, bx
    mov bx, [eax + 44]      ; Restore es
    mov es, bx
    mov bx, [eax + 46]      ; Restore fs
    mov fs, bx
    mov bx, [eax + 48]      ; Restore gs
    mov gs, bx
    mov bx, [eax + 50]      ; Restore ss
    mov ss, bx
    
    ; Restore eflags
    mov ebx, [eax + 36]
    push ebx
    popfd
    
    ; Restore general purpose registers
    mov ebx, [eax + 0]      ; Restore ebx
    mov ecx, [eax + 8]      ; Restore ecx
    mov edx, [eax + 12]     ; Restore edx
    mov esi, [eax + 16]     ; Restore esi
    mov edi, [eax + 20]     ; Restore edi
    mov ebp, [eax + 24]     ; Restore ebp
    mov esp, [eax + 28]     ; Restore esp
    
    ; Jump to new eip
    jmp [eax + 32]
