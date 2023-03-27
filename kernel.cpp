extern "C" int kmain(int text_color);
__declspec(naked) void startup()
{
    __asm 
    {
        call kmain;
    }
}

void clear();
void cursor_moveto(unsigned int strnum, unsigned int pos);
void info(void);
int length(char* s);
long int str_to_int(char* str);
char* posixtime(long int sec);
void wintime(char *data);
char *wintime_crop(char *data);
char* subtract(char* num1, char* num2);
void nsconv(char *num, int base1, int base2);
long int convert_to_dec(char* num, int base);
char* convert_dec_to_other(long int num, int base);

#define VIDEO_BUF_PTR (0xb8000)

#define WINTIME_DIFF 11644473600
#define MAX_STR_LEN 40

int color; // цвет шрифта
int strnum = 0; // номер след выводимой строки
int strpos = 0; // позиция на строке
char str[MAX_STR_LEN] = {0x00}; // строка на вывод
int str_ptr = 0; // позиция след символа на строке

void out_str(int color, const char* ptr, unsigned int strnum)
{
    unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
    video_buf += 80 * 2 * strnum;
    while (*ptr)
    {
        video_buf[0] = (unsigned char) *ptr; // Символ (код)
        video_buf[1] = color; // Цвет символа и фона
        video_buf += 2;
        ptr++;
    }
}

void clear_screen()
{
    unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
    int step = 0;
    int i = 0;

    while(step < 80 * 25)
    {
        video_buf[i] = 0x00;
        video_buf[++i] = 0x0B;
        i++;
        step++;
    }
}


// Структура описывает данные об обработчике прерывания
#pragma pack(push, 1) // Выравнивание членов структуры запрещено
struct idt_entry
{
    unsigned short base_lo; // Младшие биты адреса обработчика
    unsigned short segm_sel; // Селектор сегмента кода
    unsigned char always0; // Этот байт всегда 0
    unsigned char flags; // Флаги тип. Флаги: P, DPL, Типы - это константы - IDT_TYPE...
    unsigned short base_hi; // Старшие биты адреса обработчика
};


// Структура, адрес которой передается как аргумент команды lidt
struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
};

#define GDT_CS (0x8)
#define IDT_TYPE_INTR (0x0E)
struct idt_entry g_idt[256]; // Реальная таблица IDT
struct idt_ptr g_idtp;
#pragma pack(pop)


typedef void (*intr_handler)();
void intr_reg_handler(int num, unsigned short segm_sel, unsigned short
                        flags, intr_handler hndlr)
{
    unsigned int hndlr_addr = (unsigned int) hndlr;
    g_idt[num].base_lo = (unsigned short) (hndlr_addr & 0xFFFF);
    g_idt[num].segm_sel = segm_sel;
    g_idt[num].always0 = 0;
    g_idt[num].flags = flags;
    g_idt[num].base_hi = (unsigned short) (hndlr_addr >> 16);
}


__declspec(naked) void default_intr_handler()
{
    __asm
    {
        pusha
    }
    // ... (реализация обработки)
    __asm
    {
        popa
        iretd
    }
}


void intr_init()
{
    int i;
    int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
    for(i = 0; i < idt_count; i++)
        intr_reg_handler(i, GDT_CS, 0x80 | IDT_TYPE_INTR,
                        default_intr_handler); // segm_sel=0x8, P=1, DPL=0, Type=Intr
}


void intr_start()
{
    int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
    g_idtp.base = (unsigned int) (&g_idt[0]);
    g_idtp.limit = (sizeof (struct idt_entry) * idt_count) - 1;
    __asm
    {
        lidt g_idtp
    }
    //__lidt(&g_idtp);
}


void intr_enable()
{
    __asm sti;
}


void intr_disable()
{
    __asm cli;
}


__inline unsigned char inb (unsigned short port)
{
    unsigned char data;
    __asm
    {
        push dx
        mov dx, port
        in al, dx
        mov data, al
        pop dx
    }

    return data;
}


__inline void outb (unsigned short port, unsigned char data)
{
    __asm
    {
        push dx
        mov dx, port
        mov al, data
        out dx, al
        pop dx
    }
}

__inline void outw (unsigned short port, unsigned short data)
{
    __asm
    {
        push dx
        mov dx, port
        mov ax, data
        out dx, ax
        pop dx
    }
}


#define ERROR_GET_SYMBOL 'Q'

unsigned char get_symbol(unsigned char scan_code)
{
    switch(scan_code)
    {
        case 2:
            return '1';
        case 3:
            return '2';
        case 4:
            return '3';
        case 5:
            return '4';
        case 6:
            return '5';
        case 7:
            return '6';
        case 8:
            return '7';
        case 9:
            return '8';
        case 10:
            return '9';
        case 23:
            return 'i';
        case 49:
            return 'n';
        case 33:
            return 'f';
        case 24:
            return 'o';
        case 31:
            return 's';
        case 46:
            return 'c';
        case 47:
            return 'v';
        case 25:
            return 'p';
        case 45:
            return 'x';
        case 20:
            return 't';
        case 50:
            return 'm';
        case 18:
            return 'e';
        case 17:
            return 'w';
        case 44:
            return 'z';
        case 57:
            return ' ';
        case 38:
            return 'l';
        case 30:
            return 'a';
        case 19:
            return 'r';
        case 11:
            return '0';
        case 35:
            return 'h';
        case 32:
            return 'd';
        case 22:
            return 'u';
        case 16:
            return 'q';
        case 21:
            return 'y';
        case 34:
            return 'g';
        case 36:
            return 'j';
        case 37:
            return 'k';
        case 48:
            return 'b';
        default:
            return ERROR_GET_SYMBOL;
    }
}


void malloc(char *buffer, int size)
{
    for(int i = 0; i < size; ++i)
        buffer[i] = 0x00;
}


void unknown_command()
{
    out_str(color, "Error: Uknown command...", ++strnum);
    // strnum++;
}


char *get_input_number(char *data, int len, int ptr)
{
    char ret[32];
    int ret_ptr = 0;
    malloc(ret, 32);
    if(data[ptr] == '0')
        ptr++;
    while(data[ptr] != 0x00)
    {
        ret[ret_ptr] = data[ptr];
        ptr++;
        ret_ptr++;
    }

    return ret;
}


int check_posixtime_str(char *data)
{
    int len = length(data);
    for(int i = 0; i < len; ++i)
    {
        if(data[i] < '0' || data[i] > '9') return 2;
    }
    int f_ptr = 0;

    for(int i = 0; i < len; ++i)
    {
        if(data[i] == '0') f_ptr++;
        else break;
    }

    if(f_ptr != 0)
    {
        for(int i = f_ptr; i < len; ++i)
        {
            data[i - f_ptr] = data[i];
        }
        len -= f_ptr;
    }

    if(len > 19) return 0;
    else if(len == 19)
    {
        if(data[1] > '2') return 0;
        if(data[1] <= '2' && data[2] > '2') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] > '3') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] > '3') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] <= '3' && data[5] > '7') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] <= '3' && data[5] <= '7' && data[6] > '2') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] <= '3' && data[5] <= '7' && data[6] <= '2'
            && data[7] > '0') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] <= '3' && data[5] <= '7' && data[6] <= '2'
            && data[7] <= '0' && data[8] > '3') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] <= '3' && data[5] <= '7' && data[6] <= '2'
            && data[7] <= '0' && data[8] <= '3' && data[9] > '6') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] <= '3' && data[5] <= '7' && data[6] <= '2'
            && data[7] <= '0' && data[8] <= '3' && data[9] <= '6' && data[10] > '8') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] <= '3' && data[5] <= '7' && data[6] <= '2'
            && data[7] <= '0' && data[8] <= '3' && data[9] <= '6' && data[10] <= '8' && data[11] > '5') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] <= '3' && data[5] <= '7' && data[6] <= '2'
            && data[7] <= '0' && data[8] <= '3' && data[9] <= '6' && data[10] <= '8' && data[11] <= '5' && data[12] > '4') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] <= '3' && data[5] <= '7' && data[6] <= '2'
            && data[7] <= '0' && data[8] <= '3' && data[9] <= '6' && data[10] <= '8' && data[11] <= '5' && data[12] <= '4'
            && data[13] > '7') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] <= '3' && data[5] <= '7' && data[6] <= '2'
            && data[7] <= '0' && data[8] <= '3' && data[9] <= '6' && data[10] <= '8' && data[11] <= '5' && data[12] <= '4'
            && data[13] <= '7' && data[14] > '7') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] <= '3' && data[5] <= '7' && data[6] <= '2'
            && data[7] <= '0' && data[8] <= '3' && data[9] <= '6' && data[10] <= '8' && data[11] <= '5' && data[12] <= '4'
            && data[13] <= '7' && data[14] <= '7' && data[15] > '5') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] <= '3' && data[5] <= '7' && data[6] <= '2'
            && data[7] <= '0' && data[8] <= '3' && data[9] <= '6' && data[10] <= '8' && data[11] <= '5' && data[12] <= '4'
            && data[13] <= '7' && data[14] <= '7' && data[15] <= '5' && data[16] > '8') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] <= '3' && data[5] <= '7' && data[6] <= '2'
            && data[7] <= '0' && data[8] <= '3' && data[9] <= '6' && data[10] <= '8' && data[11] <= '5' && data[12] <= '4'
            && data[13] <= '7' && data[14] <= '7' && data[15] <= '5' && data[16] <= '8' && data[17] > '0') return 0;
        if(data[1] <= '2' && data[2] <= '2' && data[3] <= '3' && data[4] <= '3' && data[5] <= '7' && data[6] <= '2'
            && data[7] <= '0' && data[8] <= '3' && data[9] <= '6' && data[10] <= '8' && data[11] <= '5' && data[12] <= '4'
            && data[13] <= '7' && data[14] <= '7' && data[15] <= '5' && data[16] <= '8' && data[17] <= '0' && data[18] > '7') return 0;
    }

    return 1;
}


char *num;

int check_wintime_str()
{
    out_str(color, num, ++strnum);

    int len = length(num);
    for(int i = 0; i < len; ++i)
    {
        if(num[i] < '0' || num[i] > '9') return 2;
    }
    int f_ptr = 0;

    for(int i = 0; i < len; ++i)
    {
        if(num[i] == '0') f_ptr++;
        else break;
    }

    if(f_ptr != 0)
    {
        for(int i = f_ptr; i < len; ++i)
        {
            num[i - f_ptr] = num[i];
        }
        len -= f_ptr;
    }

    if(len > 18) return 0;

    return 1;
}


int wintime_flag = 0;
int nsconv_ptr = 0;


void shutdown()
{
    outw(0x604, 0x2000);
}


int convert_base(char data[2], int len)
{
    int res = 0;
    if(len == 2)
    {
        res += data[1] - '0';
        res += (data[0] - '0') * 10;
    }
    else
    {
        res += data[0] - '0';
    }
    
    return res;
}


int check_valid_base(char *data, int base)
{
    int data_len = length(data);
    if(base <= 10)
    {
        char min_sym = '0';
        char max_sym = base + '0';
        for(int i = 0; i < data_len; ++i)
        {
            if(!(data[i] >= min_sym && data[i] < max_sym))
                return 0;
        }
        return 1;
    }
    else
    {
        char max_sym = (base - 10) + 'a';
        for(int i = 0; i < data_len; ++i)
        {
            if(!(data[i] >= '0' && data[i] <= '9') && !(data[i] < max_sym && data[i] >= 'a'))
                return 0;
        }
        return 1;
    }

    return 1;
}


void choose_action(char *data, int len)
{

        // INFO
    if(data[0] == 'i' && data[1] == 'n' && data[2] == 'f' && data[3] == 'o')
    {
        for(int i = 4; i < MAX_STR_LEN; ++i)
        {
            if(data[i] != 0x00 && data[i] != ' ')
            {
                unknown_command();
                return;
            }
        }
        ++strnum;
        info();
    }
    else if(data[0] == 's' && data[1] == 'h' && data[2] == 'u' && data[3] == 't'
            && data[4] == 'd' && data[5] == 'o' && data[6] == 'w' && data[7] == 'n')
    {
        for(int i = 8; i < 40; ++i)
        {
            if(data[i] != ' ' && data[i] != 0x00)
            {
                out_str(color, "Error: invalid command...", ++strnum);
                return;
            }
        }
        out_str(color, "Powering off...", ++strnum);
        shutdown();
    }
        // POSIXTIME
    else if(data[0] == 'p' && data[1] == 'o' && data[2] == 's' && data[3] == 'i' && data[4] == 'x'
            && data[5] == 't' && data[6] == 'i' && data[7] == 'm' && data[8] == 'e')
    {
        if(data[9] != ' ')
        {
            out_str(color, "Error: invalid command or parameters...", ++strnum);
        }
        else
        {
            int flag = 0;
            num = get_input_number(data, len, 10);

            int len = length(num);
            for(int i = 0; i < len; ++i)
            {
                if(num[i] < '0' || num[i] > '9') flag = 2;
            }
            int f_ptr = 0;

            for(int i = 0; i < len; ++i)
            {
                if(num[i] == '0') f_ptr++;
                else break;
            }

            if(f_ptr != 0)
            {
                for(int i = f_ptr; i < len; ++i)
                {
                    num[i - f_ptr] = num[i];
                }
                len -= f_ptr;
            }

            if(len > 19) flag = 0;

            if(len > 11)
            {
                out_str(color, "Error: integer overflow...", ++strnum);
            }
            else if(flag == 2)
            {
                out_str(color, "Error: invalid parameters...", ++strnum);
            }
            else
            {
                long int number = str_to_int(num);
                char *ans;
                ans = posixtime(number);
                out_str(color, ans, ++strnum);
            }
            malloc(num, 32);
        }
    }
        // CLEAR   
    else if(data[0] == 'c' && data[1] == 'l' && data[2] == 'e' && data[3] == 'a' && data[4] == 'r')
    {
        for(int i = 5; i < MAX_STR_LEN; ++i)
        {
            if(data[i] != 0x00 && data[i] != ' ')
            {
                unknown_command();
                return;
            }
        }

        clear();
    }
        // NSCONV
    else if(data[0] == 'n' && data[1] == 's' && data[2] == 'c' && data[3] == 'o' && data[4] == 'n' && data[5] == 'v')
    {
        if(data[6] != ' ')
        {
            out_str(color, "Error: invalid command or parameters...", ++strnum);
            return;
        }
        else
        {
            int base1_len = 0; int base2_len = 0;
            char *num1;
            int len = length(data);
            nsconv_ptr = 7;
            int ptr = 0;
            while(data[nsconv_ptr] != ' ' && nsconv_ptr < 40)
            {
                num1[ptr] = data[nsconv_ptr];
                ptr++; nsconv_ptr++;
            }
            num1[ptr] = '\0';
            char base1[2]; char base2[2];
            if(data[nsconv_ptr] == ' ')
            {
                nsconv_ptr++;
                if(data[nsconv_ptr] >= '0' && data[nsconv_ptr] <= '9')
                {
                    base1[0] = data[nsconv_ptr];
                }
                nsconv_ptr++;
                if(data[nsconv_ptr] >= '0' && data[nsconv_ptr] <= '9')
                {
                    base1_len = 2;
                    base1[1] = data[nsconv_ptr];
                    base1[2] = 0x00;
                    nsconv_ptr++;
                }
                else
                {
                    base1_len = 1;
                    base1[1] = 0x00;
                    base1[2] = 0x00;
                }
                if(data[nsconv_ptr] == ' ')
                {
                    nsconv_ptr++;
                    if(data[nsconv_ptr] >= '0' && data[nsconv_ptr] <= '9')
                    {
                        base2[0] = data[nsconv_ptr];
                    }
                    nsconv_ptr++;
                    if(data[nsconv_ptr] >= '0' && data[nsconv_ptr] <= '9')
                    {
                        base2_len = 2;
                        base2[1] = data[nsconv_ptr];
                        base2[2] = '\0';
                        nsconv_ptr++;
                    }
                    else
                    {
                        base2_len = 1;
                        base2[1] = 0x00;
                        base2[2] = 0x00;
                    }
                    nsconv_ptr++;
                    for(int i = nsconv_ptr; i < len; ++i)
                    {
                        if(data[i] != 0x00 || data[i] != ' ')
                        {
                            out_str(color, "Error: invalid parameters...", ++strnum);
                            return;
                        }
                    }
                    // out_str(color, num1, ++strnum);
                    // out_str(color, base1, ++strnum);
                    // out_str(color, base2, ++strnum);

                        // ОСНОВНАЯ ОБРАБОТКА
                    int b1 = convert_base(base1, base1_len);
                    int b2 = convert_base(base2, base2_len);
                    if(b1 < 2 || b1 > 36 || b2 < 2 || b2 > 36)
                    {
                        out_str(color, "Error: invalid number system value...", ++strnum);
                        return;
                    }
                        // проверка допустимых введенных символов
                    int is_base_valid = check_valid_base(num1, b1);
                    if(!is_base_valid)
                    {
                        out_str(color, "Error: invalid symbol...", ++strnum);
                        return;
                    }
                        // // // / // / //  / 
                    if(b1 == b2)
                    {
                        out_str(color, num1, ++strnum);
                        return;
                    }

                    if(length(num1) > 9)
                        {
                            out_str(color, "Error: integer overflow...", ++strnum);
                            return;
                        }
                        int result = convert_to_dec(num1, b1);
                        char *fin = convert_dec_to_other(result, b2);

                        out_str(color, fin, ++strnum);
                        return;
                }
                else
                {
                    out_str(color, "Error: invalid parameters...", ++strnum);
                    return;
                }
            }
            else
            {
                out_str(color, "Error: invalid parameters...", ++strnum);
                return;
            }
        }
    }
        // WINTIME
    else if(data[0] == 'w' && data[1] == 'i' && data[2] == 'n' && data[3] == 't' && data[4] == 'i' && data[5] == 'm' && data[6] == 'e')
    {
        if(data[7] != ' ')
        {
            out_str(color, "Error: invalid command or parameters...", ++strnum);
            return;
        }
        else
        {
            // char *num;
            malloc(num, 32);
            num = get_input_number(data, len, 8);

            int flag = 1;
                /////////
            
            int len = length(num);
            for(int i = 0; i < len; ++i)
            {
                if(num[i] < '0' || num[i] > '9') flag = 2;
            }

            int f_ptr = 0;

            for(int i = 0; i < len; ++i)
            {
                if(num[i] == '0') f_ptr++;
                else break;
            }

            if(f_ptr != 0)
            {
                for(int i = f_ptr; i < len; ++i)
                {
                    num[i - f_ptr] = num[i];
                }
                len -= f_ptr;
            }

            if(flag == 2)
            {
                out_str(color, "Error: Invalid parameters, type only numbers...", ++strnum);
                return;
            }
            int f = 1;
            num[len - 7] = '\0';
            len -= 7;
            if(len < 11)
            {
                out_str(color, "Error: invalid parameters...", ++strnum);
            }
            else if(len == 11)
            {
                if(num[0] > '1') f = 0;
                else if (num[0] <= '1' && num[1] > '1') f = 0;
                else if (num[0] <= '1' && num[1] <= '1' && num[2] > '6') f = 0;
                else if (num[0] <= '1' && num[1] <= '1' && num[2] <= '6' && num[3] > '4') f = 0;
                else if (num[0] <= '1' && num[1] <= '1' && num[2] <= '6' && num[3] <= '4' && num[4] > '4') f = 0;
                else if (num[0] <= '1' && num[1] <= '1' && num[2] <= '6' && num[3] <= '4' && num[4] <= '4' && num[5] > '4') f = 0;
                else if (num[0] <= '1' && num[1] <= '1' && num[2] <= '6' && num[3] <= '4' && num[4] <= '4' && num[5] <= '4'
                         && num[6] > '7') f = 0;
                else if (num[0] <= '1' && num[1] <= '1' && num[2] <= '6' && num[3] <= '4' && num[4] <= '4' && num[5] <= '4' 
                    && num[6] <= '7' && num[7] > '3') f = 0;
                else if (num[0] <= '1' && num[1] <= '1' && num[2] <= '6' && num[3] <= '4' && num[4] <= '4' && num[5] <= '4' 
                    && num[6] <= '7' && num[7] <= '3' && num[8] > '6') f = 0;
                else if (num[0] <= '1' && num[1] <= '1' && num[2] <= '6' && num[3] <= '4' && num[4] <= '4' && num[5] <= '4' 
                    && num[6] <= '7' && num[7] <= '3' && num[8] <= '6' && num[9] > '0') f = 0;
                else if (num[0] <= '1' && num[1] <= '1' && num[2] <= '6' && num[3] <= '4' && num[4] <= '4' && num[5] <= '4' 
                    && num[6] <= '7' && num[7] <= '3' && num[8] <= '6' && num[9] <= '0' && num[10] > '0') f = 0;
            }
            if(f == 1)
            {
                out_str(color, "Error: invalid parameters...", ++strnum);
                return;
            }

            char *num2 = "11644473600";
            int len1 = 0, len2 = 11;
            len1 = len;

            char result[32] = { 0 };

            int carry = 0;
            int i, j, k = 0;

            // начинаем с конца строк
            for (i = len1 - 1, j = len2 - 1; i >= 0 || j >= 0; i--, j--)
            {
                int x = (i >= 0) ? num[i] - '0' : 0;
                int y = (j >= 0) ? num2[j] - '0' : 0;

                int sub = (x - y - carry);
                if (sub < 0)
                {
                    sub += 10;
                    carry = 1;
                }
                else
                {
                    carry = 0;
                }
                result[k++] = sub + '0';
            }
            // если остался перенос
            if (carry == 1)
            {
                result[k++] = '1';
            }
            result[k] = 0x00;

            // инвертируем строку
            int n = length(result);

            for (i = 0; i < n / 2; i++) {
                char temp = result[i];
                result[i] = result[n - i - 1];
                result[n - i - 1] = temp;
            }
            // убираем ведущие нули
            i = 0;
            while (result[i] == '0') {
                i++;
            }
            if (i == n) {
                result[0] = '0';
                result[1] = '\0';
            }
            else {
                for (j = 0; j < n - i; j++) {
                    result[j] = result[j + i];
                }
                result[j] = '\0';
            }

            long int number = str_to_int(result);
            char *ans;
            ans = posixtime(number);
            out_str(color, ans, ++strnum);

            malloc(num, 32);
        }
    }
    else
    {
        unknown_command();
    }
}


#define PIC1_PORT (0x20)

int no_command = 1;
void on_key(unsigned char scan_code)
{
    if(strnum > 20)
    {
        clear();
    }

    unsigned char sym;
    if(strpos <= 3 && scan_code == 14) // backspace + ничего не введено
        return;
    
    if(strpos >= 43)
    {
        // слишком длинная строка
        // обработка
    }

    if(no_command) // команда не введена (enter не был нажат)
        // начинаем запрос ввода команды
    {
        str[0] = '>'; str[1] = '>'; str[2] = ' '; str[3] = 0x00;
        // out_str(color, str, strnum);
        strpos = 3;
        cursor_moveto(strnum, strpos);
        no_command = 0;
    }

    sym = get_symbol(scan_code);

    if(scan_code == 14) // backspace
        // стираем последний введенный символ
    {
        if(strpos > 3)
        {
            str[strpos - 1] = ' ';
            strpos--;
            cursor_moveto(strnum, strpos);
            out_str(color, str, strnum);
            
        }
    }
    if(scan_code == 28) // enter
    {
            // обрабатываем строку
        char command[MAX_STR_LEN];
        int command_length;
        malloc(command, MAX_STR_LEN);
        for(int i = 3; i < strpos; ++i)
        {
            command[i - 3] = str[i];
        }
        command_length = strpos - 3;
            // в command лежит обрабатываемая строка
        choose_action(command, command_length);
            // + выполняем действия
        no_command = 1;
        strnum++;
        cursor_moveto(strnum, 0);
        malloc(str, 32);
    }
    else if(sym != ERROR_GET_SYMBOL)
        // добавляем символ в строку
    {
        str[strpos] = sym;
        strpos++;
        cursor_moveto(strnum, strpos);
    }

    if(scan_code != 14)
        out_str(color, str, strnum);

}


void keyb_process_keys()
{
    // Проверка что буфер PS/2 клавиатуры не пуст (младший бит присутствует)
    if (inb(0x64) & 0x01)
    {
        unsigned char scan_code;
        unsigned char state;
        scan_code = inb(0x60); // Считывание символа с PS/2 клавиатуры
        if (scan_code < 128) // Скан-коды выше 128 - это отпускание клавиши
            on_key(scan_code);
    }
}


__declspec(naked) void keyb_handler()
{
    __asm pusha;
        // Обработка поступивших данных
    keyb_process_keys();
        // Отправка контроллеру 8259 нотификации о том, что прерывание
        // обработано. Если не отправлять нотификацию, то контроллер не будет посылать
        // новых сигналов о прерываниях до тех пор, пока ему не сообщать что
        // прерывание обработано.
    outb(PIC1_PORT, 0x20);
    __asm
    {
        popa
        iretd
    }
}


void keyb_init()
{
        // Регистрация обработчика прерывания
    intr_reg_handler(0x09, GDT_CS, 0x80 | IDT_TYPE_INTR, keyb_handler);
        // segm_sel=0x8, P=1, DPL=0, Type=Intr
        // Разрешение только прерываний клавиатуры от контроллера 8259
    outb(PIC1_PORT + 1, 0xFF ^ 0x02); // 0xFF - все прерывания, 0x02 - бит IRQ1 (клавиатура).
    // Разрешены будут только прерывания, чьи биты установлены в 0
}


    // Базовый порт управления курсором текстового экрана. Подходит для
    //большинства, но может отличаться в других BIOS и в общем случае адрес
    //должен быть прочитан из BIOS data area.
#define CURSOR_PORT (0x3D4)
#define VIDEO_WIDTH (80) // Ширина текстового экрана


void cursor_moveto(unsigned int strnum, unsigned int pos)
{
    unsigned short new_pos = (strnum * VIDEO_WIDTH) + pos;
    outb(CURSOR_PORT, 0x0F);
    outb(CURSOR_PORT + 1, (unsigned char)(new_pos & 0xFF));
    outb(CURSOR_PORT, 0x0E);
    outb(CURSOR_PORT + 1, (unsigned char)( (new_pos >> 8) & 0xFF));
}




void start_message()
{
    out_str(color, "Welcome to ConvertOS (ms C compiler edition)!", strnum++);
}


void info()
{
    out_str(color, "YASM (AT & T), MS C COMPILER", strnum++);
    out_str(color, "Poshekhonov Sergey, group: 4851004/10001", strnum++);
    if(color == 0x08)
    {
        out_str(color, "Choosen color: GRAY", strnum);
    }
    else if(color == 0x07)
    {
        out_str(color, "Choosen color: WHITE", strnum);
    }
    else if(color == 0x0E)
    {
        out_str(color, "Choosen color: YELLOW", strnum);
    }
    else if(color == 0x09)
    {
        out_str(color, "Choosen color: BLUE", strnum);
    }
    else if(color == 0x04)
    {
        out_str(color, "Choosen color: RED", strnum);
    }
    else if(color == 0x02)
    {
        out_str(color, "Choosen color: GREEN", strnum);
    }
}


void clear()
{
    cursor_moveto(0, 0);
    clear_screen();
    strnum = 0;
}


int put_to_string(int data, char* buf, int ptr)
{
    if(data == 0)
    {
        int count = 2;
        for(int i = count - 1; i >= 0; i--)
        {
            buf[ptr + i] = '0';
        }

        ptr += count;
        return ptr;
    }

    int data_ = data;
    int count = 0;
    while(data_ != 0)
    {
        data_ /= 10;
        count++;
    }

    if(count == 1)
    {
        count = 2;
        for(int i = count - 1; i >= 1; i--)
        {
            buf[ptr + i] = (data % 10) + '0';
            data /= 10;
        }
        buf[ptr] = '0';
    }
    else
    {
        for(int i = count - 1; i >= 0; i--)
        {
            buf[ptr + i] = (data % 10) + '0';
            data /= 10;
        }
    }
    ptr += count;
    return ptr;
}


#define MAX_TIME_LEN 20
char* posixtime(long int sec)
{
    char ans[MAX_TIME_LEN];

    int days_of_month[] = { 31, 28, 31, 30, 31, 30,
                          31, 31, 30, 31, 30, 31 };
    
    int days_till_now, cur_year, extra_time, extra_days, index, date, month, hours, minutes, seconds, flag = 0;
        // flag - високосный год или нет

    days_till_now = sec / (24 * 60 * 60);
    extra_time = sec % (24 * 60 * 60);
    cur_year = 1970;

        // получение текущего года
    while(1)
    {
        if(cur_year % 400 == 0 || (cur_year % 4 == 0 && cur_year % 100 != 0))
        {
            if(days_till_now < 366)
            {
                break;
            }
            days_till_now -= 366;
        }
        else
        {
            if(days_till_now < 365)
            {
                break;
            }
            days_till_now -= 365;
        }
        cur_year += 1;
    }

    extra_days = days_till_now + 1;

    if(cur_year % 400 == 0 || (cur_year % 4 == 0 && cur_year % 100 != 0))
        flag = 1;

    // месяц и день
    month = 0; index = 0;
    if(flag == 1)
    {
        while(1)
        {
            if(index == 1)
            {
                if(extra_days - 29 < 0)
                    break;
                month += 1;
                extra_days -= 29;
            }
            else
            {
                if(extra_days - days_of_month[index] < 0)
                    break;
                month += 1;
                extra_days -= days_of_month[index];
            }
            index += 1;
        }
    }
    else
    {
        while(1)
        {

            if (extra_days - days_of_month[index] < 0)
            {
                break;
            }
            month += 1;
            extra_days -= days_of_month[index];
            index += 1;
        }
    }

    // получение месяца
    if (extra_days > 0)
    {
        month += 1;
        date = extra_days;
    }
    else
    {
        if (month == 2 && flag == 1)
            date = 29;
        else
            date = days_of_month[month - 1];
    }

    // часы, минуты, секунды
    hours = extra_time / 3600;
    minutes = (extra_time % 3600) / 60;
    seconds = (extra_time % 3600) % 60;

    int ptr = 0;
    ptr = put_to_string(date, ans, ptr);
    ans[ptr] = '.';
    ptr += 1;
    ptr = put_to_string(month, ans, ptr);
    ans[ptr] = '.';
    ptr += 1;
    ptr = put_to_string(cur_year, ans, ptr);
    ans[ptr] = ' ';
    ptr += 1;
    ptr = put_to_string(hours, ans, ptr);
    ans[ptr] = ':';
    ptr += 1;
    ptr = put_to_string(minutes, ans, ptr);
    ans[ptr] = ':';
    ptr += 1;
    ptr = put_to_string(seconds, ans, ptr);
    ans[ptr] = 0x00;
    return ans;
}



long int str_to_int(char* str)
{
    int result = 0;

    while(*str != 0x00)
    {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return result;
}


char* int_to_str(long int num)
{
    char str[20];

    int len = 0; long int n;
    
    n = num;
    
    // Определяем количество цифр в числе
    while (n != 0) 
    {
        n /= 10;
        len++;
    }
    for(int i = 0; i < len; ++i)
        str[i] = '0';
    str[len] = 0x00;

    // Записываем цифры числа в строку (с конца)
    for (int i = len - 1; i >= 0; i--)
    {
        str[i] = n % 10 + '0';
        n /= 10;
    }
    
    str[len] = '\0'; // Добавляем символ конца строки
    
    return str;
}


char *wintime_crop(char *data)
{
    int ptr = 0;
    while(data[ptr] != 0x00)
        ptr++;

    data[ptr - 7] = 0x00;

    return data;
}


int length(char* s)
{
    int ptr = 0;
    while (s[ptr] != 0x00)
        ptr++;

    return ptr;
}



char* subtract(char* num1, char* num2)
{

    int len1 = 0, len2 = 0;
    while(num1[len1] != 0x00)
        len1++;
    while(num2[len2] != 0x00)
        len2++;

    char result[32] = { 0 };
    
    out_str(color, result, ++strnum);

    int carry = 0;
    int i, j, k = 0;

    // начинаем с конца строк
    for (i = len1 - 1, j = len2 - 1; i >= 0 || j >= 0; i--, j--)
    {
        int x = (i >= 0) ? num1[i] - '0' : 0;
        int y = (j >= 0) ? num2[j] - '0' : 0;

        int sub = (x - y - carry);
        if (sub < 0)
        {
            sub += 10;
            carry = 1;
        }
        else
        {
            carry = 0;
        }
        result[k++] = sub + '0';
    }
    // если остался перенос
    if (carry == 1)
    {
        result[k++] = '1';
    }
    result[k] = 0x00;


    // инвертируем строку
    int n = length(result);

    for (i = 0; i < n / 2; i++) {
        char temp = result[i];
        result[i] = result[n - i - 1];
        result[n - i - 1] = temp;
    }
    // убираем ведущие нули
    while (result[i] == '0') {
        i++;
    }
    if (i == n) {
        result[0] = '0';
        result[1] = '\0';
    }
    else {
        for (j = 0; j < n - i; j++) {
            result[j] = result[j + i];
        }
        result[j] = '\0';
    }

    return result;
}


void wintime(char *data)
{

    if(length(data) < 8)
    {
        wintime_flag = 1;
        return;
    }
    data = wintime_crop(data);

    if(length(data) < 11)
    {
        wintime_flag = 1;
        return;
    }

    char *wintime_diff = "11644473600";
    char *wintime_str = subtract(data, wintime_diff);
    // проверка на макс значение
    if(length(wintime_str) > 10)
    {
        wintime_flag = 1;
        return;
    }
    //
    out_str(color, wintime_str, ++strnum);

    // long int wtime = str_to_int(wintime_str);
    // char *time = posixtime(wtime);
}


 long int convert_to_dec(char* num, int base)
 {
     long int decimal = 0;
     long int power = 1;
     
     int size = length(num);

         // Идем по числу справа налево, умножая каждую цифру на основание системы счисления, возведенное в степень
     for (int i = size - 1; i >= 0; i--)
     {
         int digit;
         if (num[i] >= '0' && num[i] <= '9')
             digit = num[i] - '0';
         else
            digit = num[i] - 'a' + 10;
         // Прибавляем значение цифры с учетом ее веса
         decimal += digit * power;
         // Увеличиваем степень
         power *= base;
     }
     return decimal;
 }


void reverse(char *s)
{
    char temp;

    int len = length(s);

    for (int i = 0, j = len - 1; i < j; ++i, --j)
    {
        temp = s[j];
        s[j] = s[i];
        s[i] = temp;
    }
}


#define MAX_LEN 32
char* convert_dec_to_other(long int num, int base)
{
    int ost = 0;
    int ptr = 0;
    char ans[MAX_LEN];

    while(num != 0)
    {
        ost = num % base;
        if(ost < 10)
             ans[ptr] = ost + '0';
        else
        {
            ans[ptr] = (ost - 10) + 'a';
        }
        ptr++;
        num /= base;
    }
    ans[ptr] = 0x00;

        // переворот числа
    reverse(ans);

    return ans;
}

void nsconv(char *num, int base1, int base2)
{
    int deci = convert_to_dec(num, base1);
    char* ans = convert_dec_to_other(deci, base2);

    out_str(color, ans, strnum++);
}

extern "C" int kmain(int text_color)
{
    color = text_color;

    intr_disable();
    intr_init();
    keyb_init();
    intr_start();
    intr_enable();
        // очистка экрана
    clear_screen();
    start_message();
    out_str(color, "Press any key to continue...", strnum++);
    cursor_moveto(1, 27);
        // бесконечный цикл
    while(1)
    {
        __asm hlt;
    }

    return 0;
}
