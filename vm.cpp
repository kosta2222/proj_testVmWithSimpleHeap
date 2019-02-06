#include "vm.h"

/** вызвать пользовательскую функцию
 \param [in] funcid индификатор функции
 \param [in] argc количество аргументов
 \param [in] массив целых аргументов
 \return значение
 */
float
call_user (int funcid, int argc, float *argv)
{
  float ret = 0;
  int i;

  if (funcid == 0)
    {
      printf ("Called user function 0 => stop.\n");
      return ret;
    }
  if (funcid == 1)
    {
      ret = cos (argv[-1]);
    }
  if (funcid == 2)
    {
      ret = sin (argv[-1]);
    }
  printf ("Called user function %d with %d args:", funcid, argc);
  for (i = 0; i < argc; i++)
    {
      printf (" %f", argv[i]);

    }
  printf ("\n");
  return ret;

}

/**
Отпечатка инструкции
 */
typedef struct
{
  /** имя инструкции*/
  char name[20];
  /** количество аргументов*/
  int nargs;
} VM_INSTRUCTION;

/**Массив данных о каждой инструкции */

static VM_INSTRUCTION vm_instructions[] = {
  { "noop", 0},
  { "iadd", 0},
  { "isub", 0},
  { "imul", 0},
  {"idiv", 0},
  {"irem", 0},
  {"ipow", 0},
  { "ilt", 0},
  { "ieq", 0},
  { "br", 1},
  { "brt", 1},
  { "brf", 1},
  { "iconst", 1},
  { "load", 1},
  { "gload", 1},
  { "store", 1},
  { "gstore", 1},
  { "print", 0},
  { "pop", 0},
  { "call", 2},
  { "ret", 0},
  {"store_result", 1},
  {"load_result", 0},
  {"INVOKE_BY_ORDINAL", 0},
  {"CREATE_STRING", 0},
  {"NEWARRAY", 0}, // создать массив по длине
  {"IASTORE", 0}, // записать число в массив
  {"IASTORE", 0}, // загрузить целое из массива целых
  {"DUP", 0}, // дублировать вершину стека
  {"ASTORE", 0}, // сохранить обьект
  {"ALOAD", 0},
  { "halt", 0}
};
/**Инициализация контекста */
static void vm_context_init (Context *ctx, int ip, int nlocals);

void
vm_init (VM *vm, unsigned char *code, int code_size, int nglobals)
{
  vm->code = code;
  vm->code_size = code_size;
  vm->globals = (float*) calloc (nglobals, sizeof (int));
  vm->nglobals = nglobals;
}

void
vm_free (VM *vm)
{
  free (vm->globals);
  free (vm);
}

VM *
vm_create (unsigned char *code, int code_size, int nglobals)
{
  VM *vm = (VM*) calloc (1, sizeof (VM));
  vm_init (vm, code, code_size, nglobals);
  return vm;
}
// *********
Variable * m_objectMap[100];
u4 m_nNextObjectID = 0;

Object
createNewArray (u4 type, u4 count)
{
  Object object;
  object.heapPtr = NULL;
  object.type = 0;

  // Создаем массив
  Variable *obj = (Variable*) malloc (sizeof (Variable)*(count + 1));

  // Добавляем обьект в "карту"
  if (obj)
    {
      memset (obj, 0, sizeof (Variable) * (count + 1));
      object.heapPtr = m_nNextObjectID++;
      obj[0].intValue = type;
      m_objectMap[object.heapPtr] = obj;
    }


  return object;
}

void
dumpHeap ()
{
  // обработать "карту"
  for (int i = 0; i < 10; i++) // возьмем 10 указателей
    {
      printf ("key %d=>\n", i);

      Variable* ptrElem = m_objectMap[i]; // получаем указатель на массив из "карты"
      for (int i = 0; i < 4; i++) // Надо отпечатать этот массив
        {
          printf ("%f:", ptrElem[i].floatValue);
        }

    }

}
// *********

/**
Выпоолняе байт-коды
\param[in] vm  экземпляр Vm
\param[in] startip с какого байта выполнять опкоды
\param[in] trace если трассировка?
\param[in] numberFromLocalsAsPar использовать ли число из локальных переменных
\return число из локальных переменных
 */
float
vm_exec (VM *vm, int startip, bool trace, int returnPrintOpFromLocals_flag)
{


  int ip;
  int sp;
  int callsp;

  float a = 0;
  float b = 0;
  int addr = 0;
  int offset = 0;

  ip = startip;
  sp = -1;
  callsp = -1;
  /**
  следующая операция
   */
  /*
  #define NEXTOP() vm->code[ip]
   */
  /**
  следующий аргумент как 'собирание' 4х байтов
   */
  /*
  #define NEXTARGASI4() (ip+=4 ,(u4) ( (u4) (vm-> code[ip-4]<<24 ) | (u4) ( vm->code[ip-3]<<16 ) | (u4) (vm-> code[ip-2]<<8 ) | (u4) (vm-> code[ip-1] ) ) )
   */
  u4 opcode = vm->code[ip];
  while (opcode != HALT)
    {
      if (trace)
        {
          vm_print_instr (vm->code, ip);

        }
      printf ("number of byte-code:%d\n", opcode);
      ip++;
      switch (opcode)
        {
        case NOOP:
          break;
        case IADD:
          b = vm->stack[sp--].floatValue;
          a = vm->stack[sp--].floatValue;
          vm->stack[++sp].floatValue = a + b;
          break;
        case ISUB:
          b = vm->stack[sp--].floatValue;
          a = vm->stack[sp--].floatValue;
          vm->stack[++sp].floatValue = a - b;
          break;
        case IMUL:
          b = vm->stack[sp--].floatValue;
          a = vm->stack[sp--].floatValue;
          vm->stack[++sp].floatValue = a * b;
          break;
        case IDIV:
          b = vm->stack[sp--].floatValue;
          a = vm->stack[sp--].floatValue;
          vm->stack[++sp].floatValue = a / b;
          break;

        case IREM:
          b = vm->stack[sp--].floatValue;
          a = vm->stack[sp--].floatValue;
          vm->stack[++sp].floatValue = (int) a % (int) b;
          break;

        case IPOW:
          b = vm->stack[sp--].floatValue;
          a = vm->stack[sp--].floatValue;
          vm->stack[++sp].floatValue = pow (a, b);
          break;
          //        case ILT:
          //          b = vm->stack[sp--];
          //          a = vm->stack[sp--];
          //          vm->stack[++sp] = (a < b) ? true : false;
          //          break;
          //        case IEQ:
          //          b = vm->stack[sp--];
          //          a = vm->stack[sp--];
          //          vm->stack[++sp] = (a == b) ? true : false;
          //          break;
          //        case BR:
          //          ip = vm->code[ip];
          //          break;
          //        case BRT:
          //          addr = vm->code[ip];
          //          if (vm->stack[sp--] == true) ip = addr;
          //          break;
          //        case BRF:
          //          addr = vm->code[ip];
          //          if (vm->stack[sp--] == false) ip = addr;
          //          break;
        case ICONST:

          vm->stack[++sp].floatValue = *((float*) &vm->code[ip]);

          ip += 3;
          ip++;
          break;

          //        case LOAD:
          //          offset = vm->code[ip++];
          //          vm->stack[++sp] = vm->call_stack[callsp].locals[offset];
          //          break;
          //        case GLOAD:
          //          addr = vm->code[ip++];
          //          vm->stack[++sp] = vm->globals[addr];
          //          break;
          //        case STORE:
          //          offset = vm->code[ip++];
          //          vm->call_stack[callsp].locals[offset] = vm->stack[sp--];
          //          break;
          //        case GSTORE:
          //          addr = vm->code[ip++];
          //          vm->globals[addr] = vm->stack[sp--];
          //          break;
          //        case PRINT:
          //          {
          //            int numberFromLocalsAsPar = vm->code[ip++];
          //            float numberFromLocals = vm->call_stack[callsp].locals[numberFromLocalsAsPar];
          //            printf ("print: %f\n", numberFromLocals);
          //            if (returnPrintOpFromLocals_flag)
          //              {
          //                return numberFromLocals;
          //              }
          //
          //            break;
          //          }
          //        case POP:
          //          --sp;
          //          break;
          //        case 25 ... 25 + 15:
          //          {
          //            int argc = (int) vm->stack[sp--];
          //            float argv[argc];
          //            for (int i = 0; i < argc; i++)
          //              {
          //                argv[i] = vm->stack[sp--];
          //
          //              }
          //            a = call_user (opcode - 25, argc, argv);
          //            if (argc != 0)
          //              {
          //                vm->float_registrThatRetFunc = a;
          //
          //              }
          //
          //
          //
          //            break;
          //
          //          }
          //        case CALL:
          //          {
          //
          //            addr = vm->code[ip++];
          //            int nargs = vm->code[ip++];
          //            int I_firstarg = sp - nargs + 1;
          //            ++callsp;
          //
          //            vm_context_init (&vm->call_stack[callsp], ip, 26);
          //
          //            for (int i = 0; i < nargs; i++)
          //              {
          //                vm->call_stack[callsp].locals[i] = vm->stack[I_firstarg + i];
          //              }
          //            sp -= nargs;
          //            ip = addr;
          //            break;
          //          }
          //        case RET:
          //          {
          //            ip = vm->call_stack[callsp].returnip;
          //            callsp--;
          //            break;
          //          }
          //        case STORE_RESULT:
          //          {
          //            int int_locNum = vm->code[ip++];
          //            vm->float_registrThatRetFunc = vm->call_stack[callsp].locals[int_locNum];
          //            break;
          //          }
          //        case LOAD_RESULT:
          //          {
          //            vm->stack[++sp] = vm->float_registrThatRetFunc;
          //            break;
          //          }
        case NEWARRAY:
          {
            vm->stack[++sp].object = createNewArray (1, (int) vm->stack[sp].floatValue);
          }

        default:
          {
            printf ("invalid opcode: %d at ip=%d\n", opcode, (ip - 1));
            exit (1);
          }

        }

      if (trace) vm_print_stack (vm->stack, sp);
      opcode = vm->code[ip];
      printf ("Heap:\n");
      dumpHeap ();



    }
  if (trace)
    {
      vm_print_data (vm->globals, vm->nglobals);
    }
}

static void
vm_context_init (Context *ctx, int ip, int nlocals)
{
  if (nlocals > DEFAULT_NUM_LOCALS)
    {
      fprintf (stderr, "too many locals requested: %d\n", nlocals);
    }
  ctx->returnip = ip;
}

void
vm_print_instr (unsigned char *code, int ip)
{

  int opcode = code[ip];
  VM_INSTRUCTION *inst = &vm_instructions[opcode];
  switch (inst->nargs)
    {
    case 0:
      printf ("%04d:  %-20s", ip, inst->name);
      break;
    case 1:
      if (opcode == ICONST)
        {

          printf ("%04d: %-10s %f", ip, "iconst", *((float*) &(code[ip + 1])));
        }
      else
        {
          printf ("%04d:  %-10s%-10d", ip, inst->name, code[ip + 1]);
        }


      break;
    case 2:
      printf ("%04d:  %-10s%d,%10d", ip, inst->name, code[ip + 1], code[ip + 2]);
      break;
    case 3:
      printf ("%04d:  %-10s%d,%d,%-6d", ip, inst->name, code[ip + 1], code[ip + 2], code[ip + 3]);

      break;
    }
}

void
vm_print_stack (Variable *stack, int count)
{
  printf ("stack=[");
  for (int i = 0; i <= count; i++)
    {

      printf (" %f", stack[i].floatValue);
    }
  printf (" ]\n");
}

void
vm_print_data (float *globals, int count)
{
  printf ("Data memory:\n");
  for (int i = 0; i < count; i++)
    {

      printf ("%04d: %f\n", i, globals[i]);
    }
}

void
vm_print_locals (float *locals, int count)
{
  printf ("Locals memory:\n");
  for (int i = 0; i < count; i++)
    {

      printf ("%04d: %f\n", i, locals[i]);
    }
}

int
main ()
{
  FILE * ptrFile = fopen ("code.bin", "r");

  if (ptrFile == NULL)
    {
      fputs ("Ошибка файла", stderr);
      exit (1);
    }

  // определяем размер файла
  fseek (ptrFile, 0, SEEK_END); // устанавливаем позицию в конец файла
  long lSize = ftell (ptrFile); // получаем размер в байтах
  rewind (ptrFile); // устанавливаем указатель в конец файла

  u1 * opcodeCharBuffer = (u1*) malloc (sizeof (u1) * lSize); // выделить память для хранения содержимого файла
  if (opcodeCharBuffer == NULL)
    {
      fputs ("Ошибка памяти", stderr);
      exit (2);
    }

  size_t result = fread (opcodeCharBuffer, 1, lSize, ptrFile); // считываем файл в буфер
  if (result != lSize)
    {
      fputs ("Ошибка чтения", stderr);
      exit (3);
    }


  // завершение работы
  fclose (ptrFile);

  printf ("size of whole byte-code %d\n", sizeof (opcodeCharBuffer));
  VM *vm = vm_create (opcodeCharBuffer, sizeof (opcodeCharBuffer), 0);
  vm_exec (vm, 0, true, 0);
  vm_free (vm);
  free (opcodeCharBuffer);

  return 0;
}