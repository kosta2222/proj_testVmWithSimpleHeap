#ifndef VM_H_
#define VM_H_
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <math.h>
#include "types.h"




#define DEFAULT_STACK_SIZE      1000///<размер стека по умолчанию
#define DEFAULT_CALL_STACK_SIZE 100///<размер стека контекстов по умолчанию
#define DEFAULT_NUM_LOCALS      26 ///<количество локальных переменных  по умолчанию

/**
   опкоды операций
 */
typedef enum {
    NOOP, ///<нет операций
    FADD, ///<сложение
    FSUB, ///<вычитание
    FMUL, ///<умножение
    FDIV, ///<деление
    IREM, ///<остаток от деления
    FPOW, ///<возведение в степень
    FEQ, ///<сравнить на менше
    IEQ, ///<сравнить на равенство
    BR, ///<прыжок
    BRT, ///<прыжок при правде
    BRF, ///<прыжок при неправде
    FCONST, ///<положить константу на стек
    LOAD, ///<загрузить из таблицы локальных переменных на стек
    GLOAD, ///<загрузить из таблицы глобальных переменнных на стек
    STORE, ///<сохранить со стека в лакальные переменные
    GSTORE, ///<сохранить со стека в глобальные переменные
    PRINT, ///<печатает локальную переменную
    POP, ///<убирает вершину стека
    CALL, ///<вызывает функцию с nargs-количество аргументов:int и сколько переменных-фактических параметров ожидать на стеке:int
    RET, ///<завершает функцию
    STORE_RESULT, ///<сохранить результат функции специальный регистр
    LOAD_RESULT, ///<загрузить результат прошлой функции из специального регистра на стек
    INVOKE_BY_ORDINAL, //< вызвать функцию по номеру в виртуальной машине
    CREATE_STRING, //< создать строку в куче
    NEWARRAY, //< создать массив в куче,взяв длину со стека
    FASTORE, //<сохранить значение в массиве
    FALOAD, //< загрузить значение из массива на стек
    DUP, // < дублировать вершину стека
    ASTORE, //< сохранить ссылку на объект в массив переменных(переменные)
    ALOAD, //< загрузить ссылку на обьект на стек
    HALT ///<остановит виртуальную машину
} VM_CODE;

/**
Контекст для функции
 */
typedef struct {
    /** адрес возврата */
    int returnip;
    /** локальные переменные контекста функции */
    Variable locals[DEFAULT_NUM_LOCALS];
} Context;

/** Компонент виртуальной машины */
typedef struct {
    /**  байт код */
    unsigned char *code;
    /**  размер байт кода */
    int code_size;

    /**  глобалные переменные */
    Variable *globals;
    /**  количество глобальных переменных */
    int nglobals;

    /**  Операндовый стек */
    Variable stack[DEFAULT_STACK_SIZE];
    /**  регистр для значения от функции */
    Variable registrThatRetFunc;
    /**  стек контекстов */
    Context call_stack[DEFAULT_CALL_STACK_SIZE];
} VM;

/** создать виртуальную машину */
VM *vm_create(unsigned char *code, int code_size, int nglobals);
/** освободить память из под виртуальной машины */
void vm_free(VM *vm);
/** инициализируем виртуальную машину */
void vm_init(VM *vm, unsigned char *code, int code_size, int nglobals);
/** выполнение инструкций */
void vm_exec(VM *vm, int startip, bool trace, int returnPrintOp_flag);
/** печатаем инструкцию */
void vm_print_instr(unsigned char *code, int ip);
/** печатаем стек */
void vm_print_stack(Variable *stack, int count);
/** печатаем глобальные переменные*/
void vm_print_data(float *globals, int count);
/** вызвать пользовательскую функцию */
Variable call_user(int funcid, int argc, float *argv);

Object createNewArray (u4 type, u4 count);

f4 getf4(char *p)
{
	u4 iv;
	iv = geti4(p);
	return *((f4 *)&iv);
}

#endif
