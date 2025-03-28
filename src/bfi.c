#define NOB_IMPLEMENTATION

#include <stdio.h>
#include "../src_build/nob.h"

typedef enum {
    OP_INC = '+',
    OP_DEC = '-',
    OP_RIGHT = '>',
    OP_LEFT = '<',
    OP_OUT = '.',
    OP_IN = ',',
    OP_JUMP_IF_ZERO = '[',
    OP_JUMP_IF_NON_ZERO = ']'
} Op_Kind;

typedef struct {
    Op_Kind kind;
    size_t operand;
} Op;

typedef struct {
    Op* items;
    size_t count;
    size_t capacity;
} Ops;

typedef struct {
    Nob_String_View content;
    size_t pos;
} Lexer;

typedef struct{
    size_t* items;
    size_t count;
    size_t capacity;
} Addr_stack;

typedef struct {
    char *items;
    size_t count;
    size_t capacity;
} Memory;

bool is_bf_cmd(char cmd) {
    const char* cmds = "+-<>.,[]";
    return strchr(cmds, cmd) != NULL;
}

char lexer_next(Lexer* l) {
    while(l->pos < l->content.count && !is_bf_cmd(l->content.data[l->pos])){
        l->pos++;
    }

    if (l->pos >= l->content.count) return 0;
    return l->content.data[l->pos++];
}

int main(int argc, char **argv) {
     const char* program = nob_shift_args(&argc, &argv);

     if (argc <= 0) {
          nob_log(NOB_ERROR, "Usage: %s <input.bf>", program);
          nob_log(NOB_ERROR, "No input is provided");
     }
     
     const char* file_path = nob_shift_args(&argc, &argv);
     Nob_String_Builder sb = {0};
     
     if (!nob_read_entire_file(file_path, &sb)) return 1;
     
     Ops ops = {0};
     Addr_stack stack = {0};
     
     Lexer l = {
      .content = {
          .data = sb.items,
          .count = sb.count
      }
     };

     char c = lexer_next(&l);
     while(c)
     {
          switch (c)
          {
             case OP_INC: 
             case OP_DEC: 
             case OP_RIGHT:
             case OP_LEFT:
             case OP_IN:
             case OP_OUT: {
                 size_t count = 1;
                 char s = lexer_next(&l);  
                 
                 while(s == c){
                     count++;
                     s = lexer_next(&l);
                 }      
                 
                 Op op = {
                     .kind = c,
                     .operand = count
                 };     
                 
                 nob_da_append(&ops, op);
                 c = s;
             } break;
             
             case OP_JUMP_IF_ZERO: {
                 Op op = {
                     .kind = c,
                     .operand = 0
                    };
                     
                 nob_da_append(&ops, op);                   
                 nob_da_append(&stack, ops.count);
                 
                 c = lexer_next(&l);
             } break;    
             
             case OP_JUMP_IF_NON_ZERO: {
                if (stack.count <= 0){
                    nob_log(NOB_ERROR, "%s[%lld] Error: Unbalanced loops", file_path, l.pos);
                    return 1;
                }

                size_t addr = stack.items[--stack.count];
                
                Op op = {
                    .kind = c,
                    .operand = addr - 1
                };

                nob_da_append(&ops, op);
                ops.items[addr-1].operand = ops.count;

                c = lexer_next(&l);
             } break;
         }
     }
     
     Memory memory = {0};
     nob_da_append(&memory, 0);
     size_t head = 0;
     size_t ip = 0;

     while(ip < ops.count) {
         Op op = ops.items[ip];
         switch (op.kind)
         {             
             case OP_INC: {
                 memory.items[head] += op.operand;
                 ip++;
             } break;
             case OP_DEC: {
                 memory.items[head] -= op.operand;
                 ip++;
             } break;
             case OP_RIGHT: {
                 head += op.operand;
                 while (head >= memory.count) {
                     nob_da_append(&memory, 0);
                 }
                 ip++;
             } break;
             case OP_LEFT: {
                 if (head < op.operand)
                 {
                     printf("RUNTIME ERROR: memory underflow\n");
                     return 1;
                 }
                 
                 head -= op.operand;
                 ip++;
             } break;
             case OP_IN: {
                for (size_t i = 0; i < op.operand; i++) {
                    int input = getchar();
                    memory.items[head] = (input != EOF)? input : 0;
                }
                ip++;
             } break;
             case OP_OUT: {
                 for (size_t i = 0; i < op.operand; i++) {
                    putchar(memory.items[head]);
                 }                
                 ip++;
             } break;
             case OP_JUMP_IF_NON_ZERO: {
                if (memory.items[head] != 0) {
                    ip = op.operand;
                }
                else {
                    ip++;
                }
             } break;
             case OP_JUMP_IF_ZERO: {
                if (memory.items[head] == 0) {
                    ip = op.operand;
                }
                else {
                    ip++;
                }
             } break;
        }
     }

     getchar();
     return 0;   
}