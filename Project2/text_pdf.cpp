#include<iostream>
#include<vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "hpdf.h"

jmp_buf env;

#define FONT_PATH "../simkai.ttf"          //�����ļ�Ŀ¼
#define START_CHAR (000000)                // Unicode���� ��ʼ
#define END_CHAR (65536)                   // Unicode���� ����
#define CHARACTERS_PER_LINE 15             //ÿ�г�15������
#define LINES_PER_PAGE 20                  // ÿҳ20��
#define X_SPACEING 30                      // ÿһ��ÿ���ַ�֮��ļ��
#define Y_SPACEING 40                     


        //��Unicode���� תΪutf-8
std::vector<uint8_t> unicode_code_point_to_utf8(uint32_t code_point)
{
    std::vector<uint8_t> utf8_bytes;

    if (code_point <= 0x7F)
    {
        utf8_bytes.push_back(code_point);
    }
    else if (code_point <= 0x7FF)
    {
        utf8_bytes.push_back(0xC0 | ((code_point >> 6) & 0x1F));
        utf8_bytes.push_back(0x80 | (code_point & 0x3F));
    }
    else if (code_point <= 0xFFFF)
    {
        utf8_bytes.push_back(0xE0 | ((code_point >> 12) & 0x0F));
        utf8_bytes.push_back(0x80 | ((code_point >> 6) & 0x3F));
        utf8_bytes.push_back(0x80 | (code_point & 0x3F));
    }
    else if (code_point <= 0x10FFFF) 
    {
        utf8_bytes.push_back(0xF0 | ((code_point >> 18) & 0x07));
        utf8_bytes.push_back(0x80 | ((code_point >> 12) & 0x3F));
        utf8_bytes.push_back(0x80 | ((code_point >> 6) & 0x3F));
        utf8_bytes.push_back(0x80 | (code_point & 0x3F));     
    }
    else
    {
        std::cerr << "Invalid Unicode code point." << std::endl;
    }

    return utf8_bytes;
}

#ifdef HPDF_DLL
void  __stdcall
#else
void
#endif
error_handler(HPDF_STATUS   error_no,
    HPDF_STATUS   detail_no,
    void* user_data)
{
    printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
        (HPDF_UINT)detail_no);
    longjmp(env, 1);
}

int main(int argc, char** argv)
{
    HPDF_Doc  pdf;
    HPDF_Font font;
    HPDF_Page page;


    char fname[256];
    unsigned char buf[8]; // һ���ַ�
    unsigned int i;

    strcpy(fname, argv[0]);
    strcat(fname, ".pdf");

    pdf = HPDF_New(error_handler, NULL);
    if (!pdf) {
        printf("���ܴ���pdf�ļ�\n");
        return 1;
    }

    if (setjmp(env)) {
        HPDF_Free(pdf);
        return 1;
    }
    // ʹ�� UTF-8 ����
    HPDF_UseUTFEncodings(pdf);
    HPDF_SetCurrentEncoder(pdf, "UTF-8");
    

    // ���� simkai ����
    const char* _font = HPDF_LoadTTFontFromFile(pdf, FONT_PATH, HPDF_TRUE);
    font = HPDF_GetFont(pdf, _font, "UTF-8");

    // ��� ҳ
    page = HPDF_AddPage(pdf);
    // ����ҳ�Ĵ�С
    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

    HPDF_Page_BeginText(page);
    // ���������С
    HPDF_Page_SetFontAndSize(page, font, 12);
    // ��ȡҳ��
    float height = HPDF_Page_GetHeight(page);

    float x = X_SPACEING * 16, y = height;

    HPDF_Page_MoveTextPos(page, x, y);
    int char_count = 0;
    int idx = 0;
    for (i = START_CHAR; i <= END_CHAR; i++)
    {
        if (char_count % CHARACTERS_PER_LINE == 0)//15������
        {
            HPDF_Page_MoveTextPos(page, -X_SPACEING * 15, -40);
           
        }
        memset(buf, 0, sizeof(buf));
        // unicode ת utf-8
        std::vector<uint8_t> tmp = unicode_code_point_to_utf8(i);
        for (int j = 0; j < tmp.size(); j++)
        {
            buf[j] = tmp[j];
        }
       
        HPDF_Page_ShowText(page, (const char*)buf); //������
        HPDF_Page_MoveTextPos(page, X_SPACEING, 0); //�ƶ�x����

        x += X_SPACEING;
        char_count++;
        if (char_count % 300 == 0) // ���� 300 ���з�ҳ
        {
            HPDF_Page_EndText(page);
            char_count = 0;
            x = X_SPACEING * 16;
            y = height;
            page = HPDF_AddPage(pdf);
            HPDF_Page_BeginText(page);
            HPDF_Page_SetFontAndSize(page, font, 12);
            HPDF_Page_MoveTextPos(page, x, y);

        }
    }

    /* save the document to a file */
    HPDF_SaveToFile(pdf, fname);

    /* clean up */
    HPDF_Free(pdf);

    system("pause");
    return 0;
}
