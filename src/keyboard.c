#include "keyboard.h"
#include "trap.h"
#include "io.h"
#include "kernelio.h"

//mode 是键盘特殊键的按下状态
//表示大小写转换键caps，交换键alt，控制键ctrl，换档键shift的状态
//位7caps down
//位5右alt down，位4左alt down，位3右ctrl down，位2左ctrl down 位1右shift down
//位0左shiftdown
static uint8_t mode = 0;

// 数字锁定键num-lock 大小写转换键caps-lock 滚动锁定键scroll-lock的LED发光管状态
//位7-3不用 位2 caps-lock 位1 num-lock(初始置1，) 位0 scroll-lock
static uint8_t leds;

//当扫描码是0xe0或0xe1时，置该标志。表示其后还跟随1个或2个字符扫描码
//位1 ==1收到0xe1标志，位0 ==1收到0xe0标志，
static uint8_t e0;

//按下shift之后的映射表
static uint8_t shift_map[] = {
      0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 127,   9,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',  10,   0, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',   0, '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?',   0, '*',   0,   0,  32,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0, '-',   0,   0,   0, '+',   0,
      0,   0,   0,   0,   0,   0,   0, '>',   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0
};

//普通的映射表
static uint8_t key_map[] = {
      0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 127,   9,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',  10,   0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';','\'', '`',   0,'\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/',   0, '*',   0,  32,  16,   1,   0, '-',   0,   0,
      0, '+',   0,   0,   0,   0,   0,   0,   0, '<', '-',   1,   0,   0, '+'
};


//按下alt之后的映射表
static uint8_t alt_map[] = {
    0, 0, 0, '@', 0, '$', 0, 0, '{', '[', ']', '}', '\\', 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, '~', 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, '|', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static void do_self(uint8_t scan_code);
static void uncaps(uint8_t scan_code);
static void caps(uint8_t scan_code);
static void lshift(uint8_t scan_code);
static void rshift(uint8_t scan_code);
static void unctrl(uint8_t scan_code);
static void ctrl(uint8_t scan_code);
static void none(uint8_t scan_code);
static void num(uint8_t scan_code);
static void minus(uint8_t scan_code);
static void cursor(uint8_t scan_code);

static void func(uint8_t scan_code);
static void scroll(uint8_t scan_code);
static void alt(uint8_t scan_code);
static void unalt(uint8_t scan_code);

static void unlshift(uint8_t scan_code);
static void unrshift(uint8_t scan_code);
static void setleds();
static void kbd_wait();
static void init_kbd();

typedef void (*keyfn_ptr)();
static keyfn_ptr key_tab[] = {
    (keyfn_ptr)none,    (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self,     /* 00-03 s0 esc 1 2 */
    (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self,     /* 04-07 3 4 5 6 */
    (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self,     /* 08-0B 7 8 9 0 */
    (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self,     /* 0C-0F + ' bs tab */
    (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self,     /* 10-13 q w e r */
    (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self,     /* 14-17 t y u i */
    (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self,     /* 18-1B o p } ^ */
    (keyfn_ptr)do_self, (keyfn_ptr)ctrl,    (keyfn_ptr)do_self, (keyfn_ptr)do_self,     /* 1C-1F enter ctrl a s */
    (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self,     /* 20-23 d f g h */
    (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self,     /* 24-27 j k l | */
    (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)lshift,  (keyfn_ptr)do_self,     /* 28-2B { para lshift , */
    (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self,     /* 2C-2F z x c v */
    (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self, (keyfn_ptr)do_self,     /* 30-33 b n m , */
    (keyfn_ptr)do_self, (keyfn_ptr)minus,   (keyfn_ptr)rshift,  (keyfn_ptr)do_self,     /* 34-37 . - rshift * */
    (keyfn_ptr)alt,     (keyfn_ptr)do_self, (keyfn_ptr)caps,    (keyfn_ptr)func,        /* 38-3B alt sp caps f1 */
    (keyfn_ptr)func,    (keyfn_ptr)func,    (keyfn_ptr)func,    (keyfn_ptr)func,        /* 3C-3F f2 f3 f4 f5 */
    (keyfn_ptr)func,    (keyfn_ptr)func,    (keyfn_ptr)func,    (keyfn_ptr)func,        /* 40-43 f6 f7 f8 f9 */
    (keyfn_ptr)func,    (keyfn_ptr)num,     (keyfn_ptr)scroll,  (keyfn_ptr)cursor,      /* 44-47 f10 num scr home */
    (keyfn_ptr)cursor,  (keyfn_ptr)cursor,  (keyfn_ptr)do_self, (keyfn_ptr)cursor,      /* 48-4B up pgup - left */
    (keyfn_ptr)cursor,  (keyfn_ptr)cursor,  (keyfn_ptr)do_self, (keyfn_ptr)cursor,      /* 4C-4F n5 right + end */
    (keyfn_ptr)cursor,  (keyfn_ptr)cursor,  (keyfn_ptr)cursor,  (keyfn_ptr)cursor,      /* 50-53 dn pgdn ins del */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)do_self, (keyfn_ptr)func,        /* 54-57 sysreq ? < f11 */
    (keyfn_ptr)func,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 58-5B f12 ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 5C-5F ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 60-63 ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 64-67 ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 68-6B ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 6C-6F ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 70-73 ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 74-77 ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 78-7B ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 7C-7F ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 80-83 ? br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 84-87 br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 88-8B br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 8C-8F br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 90-93 br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 94-97 br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* 98-9B br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)unctrl,  (keyfn_ptr)none,    (keyfn_ptr)none,        /* 9C-9F br unctrl br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* A0-A3 br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* A4-A7 br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)unlshift,(keyfn_ptr)none,        /* A8-AB br br unlshift br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* AC-AF br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* B0-B3 br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)unrshift,(keyfn_ptr)none,        /* B4-B7 br br unrshift br */
    (keyfn_ptr)unalt,   (keyfn_ptr)none,    (keyfn_ptr)uncaps,  (keyfn_ptr)none,        /* B8-BB unalt br uncaps br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* BC-BF br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* C0-C3 br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* C4-C7 br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* C8-CB br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* CC-CF br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* D0-D3 br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* D4-D7 br br br br */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* D8-DB br ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* DC-DF ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* E0-E3 e0 e1 ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* E4-E7 ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* E8-EB ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* EC-EF ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* F0-F3 ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* F4-F7 ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,        /* F8-FB ? ? ? ? */
    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none,    (keyfn_ptr)none         /* FC-FF ? ? ? ? */
};

//数字小键盘数字键对应的数字ASCII码表
static uint8_t number_tab[] = {
    '7', '8', '9', ' ', '4', '5', '6', ' ', '1', '2', '3', '0' , '.'
};
//数字小键盘方向键对应的数字ASCII码表
static uint8_t cursos_tab[] = "HA5 DGC YB623" ;

void init_keyboard() {
    init_kbd();
    register_interrupt_handler(IRQ1, keyboard_handler);
}

void keyboard_handler(register_t *reg) {
    uint8_t scan_code = inb(0x60);
    if (scan_code == 0x53 && mode & 4 && mode &16) {
        outb(0x64, 0xFE);
    }

    if (scan_code == 0xE0) {
        e0 = 1;
    } else if (scan_code == 0xE1) {
        e0 = 2;
    } else {
        key_tab[scan_code](scan_code);
        e0 = 0;
    }

    outb_p(0x61, (inb_p(0x61)) | 0x80);
    outb_p(0x61, (inb_p(0x61)) & 0x7f);
}

void do_self(uint8_t scan_code)
{
    uint8_t *map;
    uint8_t ch;


    if (mode & 0x20) {        // alt
        map = alt_map;
    } else if ((mode & 0x3)) {        // shift ^ caps
        map = shift_map;
    } else {
        map = key_map;
    }

    ch = map[scan_code & 0xff];
    if (ch == 0) {
        return;
    }

    if ((mode & 0x4c)) {
        //ctrl 或者caps
        if (((mode & 0x41) == 0x41) ||
            ((mode & 0x42) == 0x42) ||
            ((mode & 0x43) == 0x43)) {
            if ((ch <= 'Z' && ch >= 'A')) {
                //shift 和 caps 都起作用，则字母小写
                ch += 0x20 ;
            }
        } else if ((ch <= 'z' && ch >= 'a')) {
            //如果只有 caps 都起作用，则字母大写
            ch -= 0x20 ;
        }

        if (mode & 0xc) {
            //如果按下ctrl,则加上ctrl标志
            ch -= 0x40;
        }
    }
    if (mode & 0x10) {
        //如果按下alt,则加上alt标志
        ch |= 0x80 ;
    }

    ch &= 0xff;
    // put_queue(ch);
    printf("%c", ch);

    return;
}

//按下
void alt(uint8_t scan_code){
    if (e0) {// right alt
        mode |= 0x20;
    } else {
        mode |= 0x10;
    }
    return;
}

//放开alt
void unalt(uint8_t scan_code) {
    if (e0) {
        mode &= 0xdf;
    } else {
        mode &= 0xef;
    }
    return;
}

//按下ctrl
void ctrl(uint8_t scan_code) {
    if (e0) {
        mode |= 0x8;
    } else {
        mode |= 0x4;
    }
    return;
}

//放开ctrl
void unctrl(uint8_t scan_code) {
    if (e0) {
        mode &= 0xF7;
    } else {
        mode &= 0xFB;
    }
    return;
}

//按下左shift
void lshift(uint8_t scan_code) {
    mode |= 0x1;
    return;
}

//放开左shift
void unlshift(uint8_t scan_code) {
    mode &= 0xfe;
    return;
}

//按下右shift
void rshift(uint8_t scan_code) {
    mode |= 0x2;
    return;
}

//放开右shift
void unrshift(uint8_t scan_code) {
    mode &= 0xfd;
    return;
}

//按下caps
void caps(uint8_t scan_code) {
    if (!(mode & 0x80)) {
        leds ^= 0x4;
        mode ^= 0x40;
        mode |= 0x80;
        setleds();
    }
    return;
}

//放开caps
void uncaps(uint8_t scan_code) {
    mode &= 0x7f;
    return;
}

//取消scroll
void scroll(uint8_t scan_code) {
    leds ^= 0x1;
    setleds();
    return;
}

void num(uint8_t scan_code) {
    leds ^= 0x2;
    setleds();
    return;
}

//方向键
void cursor(uint8_t scan_code) {
    uint8_t i;
    uint8_t ch;

    i = scan_code - 0x47 ; // 方向键从71开始

    if (i > 12) {
        return;
    }

    if ((e0 == 1) || (! (leds & 0x2)) || (mode & 0x3)) {// 小键盘上的数字键没有lock
        ch = cursos_tab[i];

        if (ch <= '9') {
            ch = '~';
        }
        //形成移动序列
        ch = ch << 16 | 0x5b1b;
    } else {                // 小键盘上的数字键
        ch = number_tab[i];
    }
    // put_queue(ch);

    return;
}

//处理功能键
void func(uint8_t scan_code)
{
    uint8_t i = scan_code - 59;     //功能从59开始

    return;
}


void minus(uint8_t scan_code)
{
    if (e0 != 1) {
        do_self(scan_code);
    } else {
        // put_queue((u32) '/');
    }
    return;
}

void setleds()
{
    kbd_wait();
    outb(0x60, 0xed);
    kbd_wait();
    outb(0x60, leds);
    return;
}

void kbd_wait()
{
    while (inb_p(0x64) & 0x2)  //kbd in buffer is full
        ;//do nothing
    return;
}


void none(uint8_t scan_code)
{
    return;
}

void init_kbd() {
    mode = 0;
    leds = 2;
    setleds();
}
