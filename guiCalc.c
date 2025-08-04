#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <iup.h>
#include "algo.h"

Ihandle *buttons[22];
int disabled[8] = {3,7,11,15,17,18,20};
int mathErr = 0;


int lockKeys(Ihandle *ih, int key){
    char *value = IupGetAttribute(ih, "VALUE");
    char lastChar = value[strlen(value) - 1];

    if ((value == NULL || strlen(value) == 0) || strchr("+-*/^%", lastChar) != NULL)
    {
        if (
        (key >= '0' && key <= '9') ||  key == '(' || key == ')' || key == 0 || key == K_BS || key == K_DEL || 
        key == K_LEFT || key == K_RIGHT)
        {
            return IUP_DEFAULT;
        }
    }
    else if (mathErr == 1)
    {
        return IUP_IGNORE;
    }
    
    else
    {
        if (
        (key >= '0' && key <= '9') || key == '+' || key == '-' || key == '*' || key == '/' || key == '^' ||
        key == '.' || key == '(' || key == ')' || key == '%' || key == 0 || key == K_BS || key == K_DEL || 
        key == K_LEFT || key == K_RIGHT
        )
        {
            return IUP_DEFAULT;
        }
    }
    

    return IUP_IGNORE;
    
}


void updateOpsButton(Ihandle *textBox, Ihandle **buttons){
    char *value = IupGetAttribute(textBox, "VALUE");
    int isEmpty = (value == NULL || strlen(value) == 0);
    int disableOps = 0;

    if (!isEmpty)
    {
        char lastChar = value[strlen(value) - 1];
        
        if (strchr("+-*/^%", lastChar) !=NULL)
        {
           disableOps = 1;
           
        }
    }
    for (int i = 0; i < 7; i++)
    {
        IupSetAttribute(buttons[disabled[i]], "ACTIVE", (isEmpty || disableOps) ? "NO" : "YES");
    }
    
    IupSetAttribute(buttons[19], "ACTIVE", disableOps ? "NO" : "YES");
    
}

int onTextChange(Ihandle *textBox){
    updateOpsButton(textBox, buttons);
}

int clearAll(Ihandle *btnClear){
    Ihandle *textBox = IupGetDialogChild(btnClear, "txtBox");

    IupSetStrAttribute(textBox, "VALUE", "");
    for (int i = 1; i < 22; i++)
    {
        IupSetAttribute(buttons[i], "ACTIVE", "YES");
    }
    mathErr = 0;
    updateOpsButton(textBox, buttons);
    return IUP_DEFAULT;
}



int appendVal(Ihandle *btn){
    char *val= IupGetAttribute(btn, "TITLE");
    Ihandle *textBox = IupGetDialogChild(btn, "txtBox");
    const char *currentText = IupGetAttribute(textBox, "VALUE");


    char buffer[2048];
    snprintf(buffer, sizeof(buffer), "%s%s", currentText ? currentText : "", val);
    printf("%s\n", buffer);

    IupSetAttribute(textBox, "VALUE", buffer);

    updateOpsButton(textBox, buttons);

    int len = strlen(buffer);  

    // Set focus and caret position at the end
    IupSetFocus(textBox);

    char setCaret[32];
    snprintf(setCaret, sizeof(setCaret), "%d:%d", len, len);
    IupSetfAttribute(textBox, "SELECTIONPOS", setCaret);
    return IUP_DEFAULT;

}

int evalExp(Ihandle *btn){

    Token tokens[2048], postfixed[2048];
    int tokenCount = 0, postCount = 0;
    
    Ihandle *textBox = IupGetDialogChild(btn, "txtBox");
    char *value = IupGetAttribute(textBox, "VALUE");

    makeTokens(value, tokens, &tokenCount);
    infixToPostfix(tokens, tokenCount, postfixed, &postCount);
    double result = evalExpr(postfixed, postCount);

    char buffer[2048];

    snprintf(buffer, sizeof(buffer), "%.10g", result);

    if (strcmp(buffer, "inf") == 0)
    {
        strcpy(buffer, "Math Error, Press AC to Continue");
        for (int i = 1; i < 22; i++)
        {
            IupSetAttribute(buttons[i], "ACTIVE", "NO");
        }
        mathErr = 1;
        
    }
    
    printf("\n%s", buffer);
    IupSetStrAttribute(textBox, "VALUE", buffer);

    return IUP_DEFAULT;

}

int customEnter(Ihandle *ih){
    char *value = IupGetAttribute(ih, "VALUE");
    char lastChar = value[strlen(value) - 1];

    if (strchr("+-*/^%", lastChar) != NULL || mathErr != 0)
    {
        return IUP_IGNORE;
    }
    else
    {
        evalExp(ih);
        return IUP_IGNORE;
    }
    
    
}

int deleteCallback(Ihandle *btn){

    Ihandle *textBox = IupGetDialogChild(btn, "txtBox");
    char *val = IupGetAttribute(textBox, "VALUE");
    int size = strlen(val);
    char buffer[size-1];

    strncpy(buffer, val, size-1);

    buffer[size-1] = '\0';
    printf("buffer: %s\n", buffer);
    
    IupSetStrAttribute(textBox, "VALUE", buffer);

    return IUP_DEFAULT;
    
}


int main(int argc, char **argv){

    IupOpen(&argc, &argv);

    Ihandle *diagBox;
    Ihandle *textBox;
    Ihandle *border;
    Ihandle *ansBox;
    
    const char *labels[22] = {
        "AC", "(", ")", "/",
        "7", "8", "9", "*",
        "4", "5", "6", "-",
        "1", "2", "3", "+",
        "0", ".", "%", "=",
        "^", "DEL"
    };

    

    for (int i = 0; i < 22; i++)
    {
        buttons[i] = IupButton(labels[i], NULL);
        IupSetAttribute(buttons[i], "SIZE", "27x18");
        IupSetAttribute(buttons[i], "FONT", "Arial, 18");
    }
    
    for (int j = 1; j < 18; j++)
    {
        IupSetCallback(buttons[j], "ACTION", (Icallback)appendVal);
        
        
    }
    IupSetCallback(buttons[20], "ACTION", (Icallback)appendVal);
    
    

    for (int k = 0; k < 7; k++)
    {
        IupSetAttribute(buttons[disabled[k]], "ACTIVE", "NO");
    }
    

    Ihandle *row1, *row2, *row3, *row4, *row5, *row6;

    row1 = IupHbox(buttons[0], buttons[1], buttons[2], buttons[3], NULL);
    row2 = IupHbox(buttons[4], buttons[5], buttons[6], buttons[7], NULL);
    row3 = IupHbox(buttons[8], buttons[9], buttons[10], buttons[11], NULL);
    row4 = IupHbox(buttons[12], buttons[13], buttons[14], buttons[15], NULL);
    row5 = IupHbox(buttons[16], buttons[17], buttons[18], buttons[19], NULL);
    row6 = IupHbox(buttons[20], buttons[21], NULL);

    // 3 7 11 15


    IupSetCallback(buttons[0], "ACTION", (Icallback)clearAll);
    IupSetCallback(buttons[19], "ACTION", (Icallback)evalExp);

    textBox = IupText(NULL);
    IupSetAttribute(textBox, "SIZE", "107x40");
    IupSetAttribute(textBox, "FONT", "Arial, 18");
    IupSetAttribute(textBox, "BORDER", "NO");
    IupSetAttribute(textBox, "ALIGNMENT", "ARIGHT");
    IupSetAttribute(textBox, "PADDING", "10x10");
    IupSetAttribute(textBox, "MULTILINE", "YES");
    IupSetAttribute(textBox, "WRAP", "WORD");
    IupSetAttribute(textBox, "SCROLLBAR", "NO");
    IupSetAttribute(textBox, "NAME", "txtBox");
    IupSetCallback(textBox, "VALUECHANGED_CB", (Icallback)onTextChange);
    IupSetCallback(textBox, "K_ANY", (Icallback)lockKeys);

    border = IupLabel(NULL);
    IupSetAttribute(border, "SEPARATOR", "HORIZONTAL");
    IupSetAttribute(border, "EXPAND", "HORIZONTAL");
    IupSetAttribute(border, "HEIGHT", "1");
    IupSetAttribute(border, "BGCOLOR", "0 0 0");


    IupSetCallback(buttons[21], "ACTION", (Icallback)deleteCallback);

    diagBox = IupDialog(IupVbox(
        textBox,
        border,
        row1,
        row2,
        row3,
        row4,
        row5,
        row6,
        NULL
    ));

    IupSetAttribute(diagBox, "SIZE", "200x280");
    IupSetAttribute(diagBox, "RESIZE", "NO");
    IupSetCallback(textBox, "K_CR", (Icallback)customEnter);

    IupShowXY(diagBox, IUP_CENTER, IUP_CENTER);


    IupMainLoop();


    IupClose();

    return EXIT_SUCCESS;
}