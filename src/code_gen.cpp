#include "code_gen.h"

#include <stack>
#include <cstring>

namespace DRCC
{

void WarningIfExist(const std::string & id, const std::map<std::string, int> & M)
{
    if(M.find(id) != M.end())
    {
        std::cerr << "[Warning] Multiple declaration of variable \"" << id << "\"\n";
    }
}

/// @brief Helper funtion lexeme -> int
int hexval(char c)
{
    if(c >= 'a')
    {
        return c - 'a' + 10;
    }

    return c - '0';
}

/// @brief Helper funtion lexeme -> int
int to_integer(const char * const s)
{
    int length = std::strlen(s);
    const char * str = s;
    int base = 10;

    if(s[0] == '0')
    {
        if(length > 2 && s[1] == 'x')
        {
            base = 16;
            str = s + 2;
            length -= 2;
        }
        else
        {
            base = 8;
        }
    }

    int ans = 0;
    for(int i = 0; i < length; i++)
    {
        ans = ans * base + hexval(str[i]);
    }
    return ans;
}

void mipsCodeGen::cgen_(std::shared_ptr<ASTNode> node_ptr, int nt, std::ostream &os)
{
    
    /**
     *  Attributes of ASTNode
     *      Symbol symbol;
     *      int prod_idx;
     *      std::string lexeme;
     *      std::vector<std::shared_ptr<ASTNode>> children; 
     * 
     */
    static std::stack<std::string> end_labels;

    int lid, offset, int_value;
    switch (node_ptr->prod_idx)
    {
    case 0: // goal -> program
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 1: // program -> var_declarations, statements
        os << "\t.data\nbreak_line:\n\t.asciiz \"\\n\"\n";
        os << "\t.text\n";
        os << "\taddiu\t$sp, $sp, " << - tot_offset << "\n";
        os << "\tmove\t$fp, $sp\n";
        while(!end_labels.empty()) end_labels.pop();
        cgen_(node_ptr->children[0], nt, os);
        cgen_(node_ptr->children[1], nt, os);
        os << "main_exit:\n";
        break;

    case 2: // var_declarations -> var_declarations, var_declaration
        cgen_(node_ptr->children[0], nt, os);
        cgen_(node_ptr->children[1], nt, os);
        break;

    case 3: // var_declarations -> /* EMPTY */
        break;

    case 4: // var_declaration -> INT, declaration_list, SEMI
        cgen_(node_ptr->children[1], nt, os);
        break;

    case 5: // declaration_list -> declaration_list, COMMA, declaration
        cgen_(node_ptr->children[0], nt, os);
        cgen_(node_ptr->children[2], nt, os);
        break;

    case 6: // declaration_list -> declaration
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 7: // declaration -> ID, ASSIGN, INT_NUM
        os << "\tli\t\t$a0, " << to_integer(node_ptr->children[2]->lexeme.c_str()) << "\n";
        os << "\tsw\t\t$a0, " << symbol_table[node_ptr->children[0]->lexeme] << "($fp)\n";
        break;

    case 8: // declaration -> ID, LSQUARE, INT_NUM, RSQUARE
        break;

    case 9: // declaration -> ID
        break;

    case 10: // code_block -> LBRACE, statements, RBRACE
        cgen_(node_ptr->children[1], nt, os);
        break;

    case 11: // statements -> statement
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 12: // statements -> statements, statement
        cgen_(node_ptr->children[0], nt, os);
        cgen_(node_ptr->children[1], nt, os);
        break;

    case 13: // statement -> open_stmt
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 14: // statement -> closed_stmt
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 15: // closed_stmt -> simple_stmt
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 16: // closed_stmt -> IF, LPAR, exp, RPAR, closed_stmt, ELSE, closed_stmt
        //      cgen(exp)
        //      bnez    $a0, if_true_label
        //      cgen(stmt2)
        //      b       end_if_label
        // if_true_label:
        //      cgen(stmt1)
        // end_if_label:
        lid = label_cnt++;
        cgen_(node_ptr->children[2], nt, os);
        os << "\tbnez\t $a0, if_true_" << lid << "\n";
        cgen_(node_ptr->children[6], nt, os);
        os << "\tb\t\tend_if_" << lid << "\n";
        os << "if_true_" << lid << ":\n";
        cgen_(node_ptr->children[4], nt, os);
        os << "end_if_" << lid << ":\n";

        break;

    case 17: // closed_stmt -> WHILE, LPAR, exp, RPAR, closed_stmt
        /**
         * while_begin:
         *      cgen(exp)
         *      beqz    $a0, while_end
         *      cgen(stmt)
         *      b       while_begin
         * while_end:
         *      
         */
        lid = label_cnt++;
        os << "while_begin_" << lid << ":\n";
        cgen_(node_ptr->children[2], nt, os);
        os << "\tbeqz\t$a0, " << "while_end_" << lid << "\n";

        end_labels.push(std::string("while_end_") + std::to_string(lid));
        cgen_(node_ptr->children[4], nt, os);
        end_labels.pop();

        os << "\tb\t\twhile_begin_" << lid << "\n";
        os << "while_end_" << lid << ":\n";

        break;

    case 18: // open_stmt -> IF, LPAR, exp, RPAR, statement
        /**
         *      cgen(exp)
         *      beqz    $a0, end_if_label
         *      cgen(stmt)
         * end_if_label:
         * 
         */
        lid = label_cnt++;
        cgen_(node_ptr->children[2], nt, os);
        os << "\tbeqz\t$a0, end_if_" << lid << "\n";
        cgen_(node_ptr->children[4], nt, os);
        os << "end_if_" << lid << ":\n";
        break;

    case 19: // open_stmt -> IF, LPAR, exp, RPAR, closed_stmt, ELSE, open_stmt
        //      cgen(exp)
        //      bnez    $a0, if_true_label
        //      cgen(stmt2)
        //      b       end_if_label
        // if_true_label:
        //      cgen(stmt1)
        // end_if_label:
        lid = label_cnt++;
        cgen_(node_ptr->children[2], nt, os);
        os << "\tbnez\t $a0, if_true_" << lid << "\n";
        cgen_(node_ptr->children[6], nt, os);
        os << "\tb\t\tend_if_" << lid << "\n";
        os << "if_true_" << lid << ":\n";
        cgen_(node_ptr->children[4], nt, os);
        os << "end_if_" << lid << ":\n";
        break;

    case 20: // open_stmt -> WHILE, LPAR, exp, RPAR, open_stmt
        /**
         * while_begin:
         *      cgen(exp)
         *      beqz    $a0, while_end
         *      cgen(stmt)
         *      b       while_begin
         * while_end:
         *      
         */
        lid = label_cnt++;
        os << "while_begin_" << lid << ":\n";
        cgen_(node_ptr->children[2], nt, os);
        os << "\tbeqz\t$a0, " << "while_end_" << lid << "\n";

        end_labels.push(std::string("while_end_") + std::to_string(lid));
        cgen_(node_ptr->children[4], nt, os);
        end_labels.pop();
        
        os << "\tb\t\twhile_begin_" << lid << "\n";
        os << "while_end_" << lid << ":\n";
        break;

    case 21: // simple_stmt -> assign_stmt, SEMI
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 22: // simple_stmt -> ctrl_stmt
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 23: // simple_stmt -> io_stmt, SEMI
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 24: // simple_stmt -> code_block
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 25: // simple_stmt -> exp, SEMI
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 26: // simple_stmt -> SEMI
        break;

    case 27: // ctrl_stmt -> do_while_stmt, SEMI
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 28: // ctrl_stmt -> return_stmt, SEMI
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 29: // io_stmt -> read_stmt
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 30: // io_stmt -> write_stmt
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 31: // assign_stmt -> ID, LSQUARE, exp, RSQUARE, ASSIGN, exp
        /**
         *      cgen(exp1)
         *      sll     $a0, $a0, 2
         *      addu    $a0, $fp, $a0
         *      sw      $a0, -4 * nt ($fp)
         *      cgen(exp2)
         *      lw      $t1, -4 * nt ($fp)
         *      sw      $a0, Offset_ID($t1)
         * 
         */
        cgen_(node_ptr->children[2], nt, os);
        os << "\tsll\t\t$a0, $a0, 2\n";
        os << "\taddu\t$a0, $fp, $a0\n";
        os << "\tsw\t\t$a0, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[5], nt + 1, os);
        os << "\tlw\t\t$t1, " << -4 * nt << "($fp)\n";
        os << "\tsw\t\t$a0, " << symbol_table[node_ptr->children[0]->lexeme] << "($t1)\n";
        break;

    case 32: // assign_stmt -> ID, ASSIGN, exp
        /**
         *      cgen(exp)
         *      sw      $a0, Offset_ID($fp)
         * 
         */
        cgen_(node_ptr->children[2], nt, os);
        os << "\tsw\t\t$a0, " << symbol_table[node_ptr->children[0]->lexeme] << "($fp)\n";
        break;

    case 33: // do_while_stmt -> DO, statement, WHILE, LPAR, exp, RPAR
        /**
         * do_while_begin:
         *      cgen(stmt)
         *      cgen(exp)
         *      bnez    $a0, do_while_begin
         * do_while_end:
         */
        lid = label_cnt++;
        os << "do_while_begin_" << lid << ":\n";
        
        end_labels.push(std::string("do_while_end_") + std::to_string(lid));
        cgen_(node_ptr->children[1], nt, os);
        end_labels.pop();

        cgen_(node_ptr->children[4], nt, os);
        os << "\tbnez\t$a0, do_while_begin_" << lid << "\n";
        os << "do_while_end_" << lid << ":\n";
        break;

    case 34: // return_stmt -> RETURN
        os << "\tj\t\tmain_exit\n";
        break;

    case 35: // read_stmt -> READ, LPAR, ID, RPAR
        /**
         *      li      $v0, 5
         *      syscall
         *      sw      $v0, Offset_ID($fp)
         * 
         */
        os << "\tli\t\t$v0, 5\n\tsyscall\n";
        os << "\tsw\t\t$v0, " << symbol_table[node_ptr->children[2]->lexeme] << "($fp)\n";
        break;

    case 36: // write_stmt -> WRITE, LPAR, exp, RPAR
        /**
         *      cgen(exp)
         *      li      $v0, 1
         *      syscall
         *      li      $v0, 4
         *      la      $a0, break_line
         *      syscall
         */
        cgen_(node_ptr->children[2], nt, os);
        os << "\tli\t\t$v0, 1\n\tsyscall\n";
        os << "\tli\t\t$v0, 4\n";
        os << "\tla\t\t$a0, break_line\n";
        os << "\tsyscall\n";
        break;

    case 37: // exp -> exp12
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 38: // exp12 -> exp12, op12, exp11
        /**
         *      cgen(exp0)
         *      bnez    $a0, one_label
         *      cgen(exp1)
         *      beqz    $a0, zero_label
         * one_label:
         *      li      $a0, 1
         *      b       end_label
         * zero_label:
         *      move    $a0, $zero
         * end_label:
         */
        lid = label_cnt++;
        cgen_(node_ptr->children[0], nt, os);
        os << "\tbnez\t$a0, one_" << lid << "\n";
        cgen_(node_ptr->children[2], nt, os);
        os << "\tbeqz\t$a0, zero_" << lid << "\n";
        os << "one_" << lid << ":\n";
        os << "\tli\t\t$a0, 1\n";
        os << "\tb\t\tend_" << lid << "\n";
        os << "zero_" << lid << ":\n";
        os << "\tmove\t$a0, $zero\n";
        os << "end_" << lid << ":\n";

        break;

    case 39: // exp12 -> exp11
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 40: // op12 -> OROR
        break;

    case 41: // exp11 -> exp11, op11, exp10
        /**
         *      cgen(exp0)
         *      beqz    $a0, zero_label
         *      cgen(exp1)
         *      beqz    $a0, zero_label
         *      li      $a0, 1
         *      b       end_label
         * zero_label:
         *      move    $a0, $zero
         * end_label:
         */
        lid = label_cnt++;
        cgen_(node_ptr->children[0], nt, os);
        os << "\tbeqz\t$a0, zero_" << lid << "\n";
        cgen_(node_ptr->children[2], nt, os);
        os << "\tbeqz\t$a0, zero_" << lid << "\n";
        os << "\tli\t\t$a0, 1\n";
        os << "\tb\t\tend_" << lid << "\n";
        os << "zero_" << lid << ":\n";
        os << "\tmove\t$a0, $zero\n";
        os << "end_" << lid << ":\n";
        break;

    case 42: // exp11 -> exp10
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 43: // op11 -> ANDAND
        break;

    case 44: // exp10 -> exp10, op10, exp8
        /**
         *      cgen(exp0)
         *      sw      $a0, -4 * nt ($fp)
         *      cgen(exp1)
         *      lw      $t1, -4 * nt ($fp)
         *      cgen(op)                        # $a0 = $t1 op $a0
         * 
         */
        cgen_(node_ptr->children[0], nt, os);
        os << "\tsw\t\t$a0, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[2], nt + 1, os);
        os << "\tlw\t\t$t1, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[1], nt, os);
        break;

    case 45: // exp10 -> exp8
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 46: // op10 -> OR_OP
        /**
         *      or      $a0, $t1, $a0
         */
        os << "\tor\t\t$a0, $t1, $a0\n";
        break;

    case 47: // exp8 -> exp8, op8, exp7
        /**
         *      cgen(exp0)
         *      sw      $a0, -4 * nt ($fp)
         *      cgen(exp1)
         *      lw      $t1, -4 * nt ($fp)
         *      cgen(op)                        # $a0 = $t1 op $a0
         * 
         */
        cgen_(node_ptr->children[0], nt, os);
        os << "\tsw\t\t$a0, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[2], nt + 1, os);
        os << "\tlw\t\t$t1, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[1], nt, os);
        break;

    case 48: // exp8 -> exp7
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 49: // op8 -> AND_OP
        /**
         *      and     $a0, $t1, $a0
         */
        os << "\tand\t\t$a0, $t1, $a0\n";
        break;

    case 50: // exp7 -> exp7, op7, exp6
        /**
         *      cgen(exp0)
         *      sw      $a0, -4 * nt ($fp)
         *      cgen(exp1)
         *      lw      $t1, -4 * nt ($fp)
         *      cgen(op)                        # $a0 = $t1 op $a0
         * 
         */
        cgen_(node_ptr->children[0], nt, os);
        os << "\tsw\t\t$a0, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[2], nt + 1, os);
        os << "\tlw\t\t$t1, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[1], nt, os);
        break;

    case 51: // exp7 -> exp6
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 52: // op7 -> EQ
        /**
         *      xor     $a0, $t1, $a0
         *      sltiu   $a0, $a0, 1
         */
        os << "\txor\t\t$a0, $t1, $a0\n";
        os << "\tsltiu\t$a0, $a0, 1\n";
        break;

    case 53: // op7 -> NOTEQ
        /**
         *      xor     $a0, $t1, $a0
         *      sltu    $a0, $zero, $a0
         */
        os << "\txor\t\t$a0, $t1, $a0\n";
        os << "\tsltu\t$a0, $zero, $a0\n";
        break;

    case 54: // exp6 -> exp6, op6, exp5
        /**
         *      cgen(exp0)
         *      sw      $a0, -4 * nt ($fp)
         *      cgen(exp1)
         *      lw      $t1, -4 * nt ($fp)
         *      cgen(op)                        # $a0 = $t1 op $a0
         * 
         */
        cgen_(node_ptr->children[0], nt, os);
        os << "\tsw\t\t$a0, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[2], nt + 1, os);
        os << "\tlw\t\t$t1, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[1], nt, os);
        break;

    case 55: // exp6 -> exp5
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 56: // op6 -> GT
        /**
         *      slt     $a0, $a0, $t1           # $a0 = $t1 > $a0
         */
        os << "\tslt\t\t$a0, $a0, $t1\n";
        break;

    case 57: // op6 -> LT
        /**
         *      slt     $a0, $t1, $a0
         */
        os << "\tslt\t\t$a0, $t1, $a0\n";
        break;

    case 58: // op6 -> GTEQ
        /**
         *      slt     $a0, $t1, $a0           # $a0 = $t1 < $a0
         *      xori    $a0, $a0, 1             # $a0 = !($t1 < $a0) = $t1 >= $a0
         */
        os << "\tslt\t\t$a0, $t1, $a0\n";
        os << "\txori\t$a0, $a0, 1\n";
        break;

    case 59: // op6 -> LTEQ
        /**
         *      slt     $a0, $a0, $t1
         *      xori    $a0, $a0, 1
         */
        os << "\tslt\t\t$a0, $a0, $t1\n";
        os << "\txori\t$a0, $a0, 1\n";
        break;

    case 60: // exp5 -> exp5, op5, exp4
        /**
         *      cgen(exp0)
         *      sw      $a0, -4 * nt ($fp)
         *      cgen(exp1)
         *      lw      $t1, -4 * nt ($fp)
         *      cgen(op)                        # $a0 = $t1 op $a0
         * 
         */
        cgen_(node_ptr->children[0], nt, os);
        os << "\tsw\t\t$a0, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[2], nt + 1, os);
        os << "\tlw\t\t$t1, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[1], nt, os);
        break;

    case 61: // exp5 -> exp4
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 62: // op5 -> SHL_OP
        /**
        *       sllv    $a0, $t1, $a0 
        */
        os << "\tsllv\t$a0, $t1, $a0\n";
        break;

    case 63: // op5 -> SHR_OP
        /**
        *       srav    $a0, $t1, $a0 
        */
        os << "\tsrav\t$a0, $t1, $a0\n";
        break;

    case 64: // exp4 -> exp4, op4, exp3
        /**
         *      cgen(exp0)
         *      sw      $a0, -4 * nt ($fp)
         *      cgen(exp1)
         *      lw      $t1, -4 * nt ($fp)
         *      cgen(op)                        # $a0 = $t1 op $a0
         * 
         */
        cgen_(node_ptr->children[0], nt, os);
        os << "\tsw\t\t$a0, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[2], nt + 1, os);
        os << "\tlw\t\t$t1, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[1], nt, os);
        break;

    case 65: // exp4 -> exp3
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 66: // op4 -> PLUS
        /**
         *      addu    $a0, $t1, $a0
         */
        os << "\taddu\t$a0, $t1, $a0\n";
        break;

    case 67: // op4 -> MINUS
        /**
         *      subu    $a0, $t1, $a0
         */
        os << "\tsubu\t$a0, $t1, $a0\n";
        break;

    case 68: // exp3 -> exp3, op3, exp2
        /**
         *      cgen(exp0)
         *      sw      $a0, -4 * nt ($fp)
         *      cgen(exp1)
         *      lw      $t1, -4 * nt ($fp)
         *      cgen(op)                        # $a0 = $t1 op $a0
         * 
         */
        cgen_(node_ptr->children[0], nt, os);
        os << "\tsw\t\t$a0, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[2], nt + 1, os);
        os << "\tlw\t\t$t1, " << -4 * nt << "($fp)\n";
        cgen_(node_ptr->children[1], nt, os);
        break;

    case 69: // exp3 -> exp2
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 70: // op3 -> MUL_OP
        /**
         *      mul     $a0, $t1, $a0
         */
        os << "\tmul\t\t$a0, $t1, $a0\n";
        break;

    case 71: // op3 -> DIV_OP
        /**
         *      bne     $a0, $zero, div_label
         *      break   7
         * div_label:
         *      div     $t1, $a0
         *      mflo    $a0
         */
        lid = label_cnt++;
        os << "\tbne\t\t$a0, $zero, div_" << lid << "\n";
        os << "\tbreak\t7\ndiv_" << lid <<":\n";
        os << "\tdiv\t\t$t1, $a0\n";
        os << "\tmflo\t$a0\n";
        break;

    case 72: // exp2 -> op2, exp2
        /**
         *      cgen(exp)
         *      cgen(op)
         */
        cgen_(node_ptr->children[1], nt, os);
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 73: // exp2 -> exp1
        cgen_(node_ptr->children[0], nt, os);
        break;

    case 74: // op2 -> PLUS
        break;

    case 75: // op2 -> MINUS
        /**
         *      subu    $a0, $zero, $a0
         */
        os << "\tsubu\t$a0, $zero, $a0\n";
        break;

    case 76: // op2 -> NOT_OP
        /**
         *      sltiu   $a0, $a0, 1
         */
        os << "\tsltiu\t$a0, $a0, 1\n";
        break;

    case 77: // exp1 -> INT_NUM
        /**
         *      li      $a0, INT_VAL
         */
        int_value = to_integer(node_ptr->children[0]->lexeme.c_str());
        os << "\tli\t\t$a0, " << int_value << "\n";
        break;

    case 78: // exp1 -> ID
        /**
         *      lw      $a0, Offset_ID($fp)
         */
        offset = symbol_table[node_ptr->children[0]->lexeme];
        os << "\tlw\t\t$a0, " << offset << "($fp)\n";
        break;

    case 79: // exp1 -> ID, LSQUARE, exp, RSQUARE
        /**
         *      cgen(exp)
         *      sll     $a0, $a0, 2
         *      addu    $a0, $a0, $fp
         *      lw      $a0, Offset_ID($a0)
         */
        offset = symbol_table[node_ptr->children[0]->lexeme];
        cgen_(node_ptr->children[2], nt, os);
        os << "\tsll\t\t$a0, $a0, 2\n";
        os << "\taddu\t$a0, $a0, $fp\n";
        os << "\tlw\t\t$a0, " << offset << "($a0)\n";
        break;

    case 80: // exp1 -> LPAR, exp, RPAR
        cgen_(node_ptr->children[1], nt, os);
        break;

    case 81: // ctrl_stmt -> break, SEMI
        /**
         *      b   last_label
         * 
         */
        os << "\tb\t\t" << end_labels.top() << "\n";
        break;

    case 82: // op3 -> MOD_OP
        /**
         *      bne     $a0, $zero, div_label
         *      break   7
         * div_label:
         *      div     $t1, $a0
         *      mfhi    $a0
         */
        lid = label_cnt++;
        os << "\tbne\t\t$a0, $zero, div_" << lid << "\n";
        os << "\tbreak\t7\ndiv_" << lid <<":\n";
        os << "\tdiv\t\t$t1, $a0\n";
        os << "\tmfhi\t$a0\n";
        break;

    default: // nonterminal
        break;
    }
}

void mipsCodeGen::build_symbol_table_(std::shared_ptr<ASTNode> node_ptr)
{
    fprintf(stderr, "[debug] productive rule index = %d\n", node_ptr->prod_idx);
    switch (node_ptr->prod_idx)
    {
    case 0: // goal -> program
        build_symbol_table_(node_ptr->children[0]);
        break;

    case 1: // program -> var_declarations, statements
        build_symbol_table_(node_ptr->children[0]);
        break;

    case 2: // var_declarations -> var_declarations, var_declaration
        build_symbol_table_(node_ptr->children[0]);
        build_symbol_table_(node_ptr->children[1]);
        break;

    case 3: // var_declarations -> 
        break;

    case 4: // var_declaration -> INT, declaration_list, SEMI
        build_symbol_table_(node_ptr->children[1]);
        break;

    case 5: // declaration_list -> declaration_list, COMMA, declaration
        build_symbol_table_(node_ptr->children[0]);
        build_symbol_table_(node_ptr->children[2]);
        break;

    case 6: // declaration_list -> declaration
        build_symbol_table_(node_ptr->children[0]);
        break;

    case 7: // declaration -> ID, ASSIGN, INT_NUM
        WarningIfExist(node_ptr->children[0]->lexeme, symbol_table);
        symbol_table[node_ptr->children[0]->lexeme] = tot_offset;
        tot_offset += 4;
        break;

    case 8: // declaration -> ID, LSQUARE, INT_NUM, RSQUARE
        WarningIfExist(node_ptr->children[0]->lexeme, symbol_table);
        symbol_table[node_ptr->children[0]->lexeme] = tot_offset;
        tot_offset += 4 * to_integer(node_ptr->children[2]->lexeme.c_str());
        break;

    case 9: // declaration -> ID
        WarningIfExist(node_ptr->children[0]->lexeme, symbol_table);
        symbol_table[node_ptr->children[0]->lexeme] = tot_offset;
        tot_offset += 4;
        break;

    default:
        break;
    }
}

mipsCodeGen::mipsCodeGen(std::shared_ptr<ASTNode> node_ptr)
    : root(node_ptr), tot_offset(4), label_cnt(0), symbol_table()
{
    if(node_ptr != nullptr)
    {
        build_symbol_table_(node_ptr);
        fprintf(stderr, "size = %d [Bytes] = %d KB \n", tot_offset, tot_offset / 1024);
    }
}

void mipsCodeGen::generate(std::ostream &os)
{
    if(this->root != nullptr)
    {
        this->cgen_(this->root, 0, os);
    }
}

}