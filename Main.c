// Include Bibliotecas
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Especificar tamanho das senhas
#define tamanhoSenha 4


// ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- //
                              // Definir Variáveis
char tecla; // Tecla lida do teclado
char digitada[10]; // Senha digitada pelo usuário para abrir a fechadrua //+1 para evitar problemas com char 
int armazenadas = 1; // Número de Senhas Armazenadas // 1 pois senhaADM vem por padrão
int contador = 0; // Conta o número vezes de tecla pressionada
int i;
bool encontrou, aberta, ADM; // Econtrou: senha consta na memória // Aberta: Estado da fechadura // ADM: Senha de ADM foi digitada corretamente


// Criar struct para armazenar as senhas cadastradas
typedef struct
{
    char senhas[tamanhoSenha + 1];
    char senhaADM[tamanhoSenha + 1];
    int posicaoDel;

}dados;

dados dado[10];

// ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- //
                                  //Parâmetros do teclado 

const byte linhas = 4;
const byte colunas = 4;

char teclas[linhas][colunas] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte linhaPinos[linhas] = { 9, 8, 7, 6 };
byte colunaPinos[colunas] = { 5, 4, 3, 2 };

Keypad teclado = Keypad(makeKeymap(teclas), linhaPinos, colunaPinos, linhas, colunas);
// ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- //

// Definir o lcd
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {

    Serial.begin(9600);

    // Armazenar senha do ADM
    dado[0].senhas[0] = '8';
    dado[0].senhas[1] = '0';
    dado[0].senhas[2] = '8';
    dado[0].senhas[3] = '0';
    dado[0].senhaADM[0] = '8';
    dado[0].senhaADM[1] = '0';
    dado[0].senhaADM[2] = '8';
    dado[0].senhaADM[3] = '0';

    // Inciar lcd
    lcd.init();
    //lcd.backlight();

}

void loop() {

    // Scanear teclado
    tecla = teclado.getKey();

    // Digitou algo 
    if (tecla != NO_KEY) {

        // Fechar fechadura // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- //
        if (contador == 0 and tecla == 'C' and aberta == true) {

            lcd.backlight();
            lcd.clear();
            lcd.home(); // posiciona o texto do lcd no ponto incial
            lcd.print("Fechadura");
            lcd.setCursor(5, 1);
            lcd.print("Fechada");
            Serial.print("\nFechadura fechada");

            delay(3000); // Espera 3 segundos e apaga o lcd
            lcd.noBacklight();
            aberta = false;
            // ADD COMANDO FECHAR FECHADURA
        }

        // Acessar Menu Administrador  // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- //
        else if (contador == 0 and tecla == 'A') {

            lcd.backlight();
            lcd.clear();
            lcd.home(); // posiciona o texto do lcd no ponto incial
            lcd.print("Senha ADM");
            Serial.print("\nDigite a senha de Administrador");
            ADM = verificarADM();

            if (ADM == true) {

                armazenadas = menuADM(armazenadas); // Função modifica o número de senhas armazenadas 

                delay(3000); // Espera 3 segundos e apaga o lcd
                lcd.noBacklight();
                Serial.print("Senhas armazenadas: ");
                Serial.print(armazenadas);
                contador = 0;
            }
            else {
                Serial.print("\nSenha de Administrador incorreta");
            }
        }

        // Terminou de digitar a senha // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- //
        else if (tecla == '#' or tecla == '*') {

            encontrou = false; // Restaura variável encontrou

            // Senha digitada tem o tamanho certo, pode ser correta // Caso contador != tamanhoSenha a senha digitada ja é dada como incorreta
            if (contador == tamanhoSenha) {

                // Verifica se a senha digitada existe
                Serial.print("\nVerificando senha");
                encontrou = verificar(digitada);

                // Senha encontrada
                if (encontrou) {

                    // COMANDO ABRIR
                    aberta = true;
                    contador = 0;

                    lcd.clear();
                    lcd.home(); // Posiciona o texto do lcd no ponto incial
                    lcd.print("Fechadura");
                    lcd.setCursor(5, 1);
                    lcd.print("Aberta");

                    delay(3000); // Espera 3 segundos e apaga o lcd
                    lcd.noBacklight();

                    Serial.print("Fechadura Aberta");
                }
            }

            // Senha inválida
            if (encontrou == false) {

                contador = 0;
                lcd.clear();
                lcd.home(); // Posiciona o texto do lcd no ponto incial
                lcd.print("Senha");
                lcd.setCursor(5, 1);
                lcd.print("Invalida");
                Serial.print("Senha inválida");

                delay(2000); // Espera 3 segundos e apaga o lcd
                lcd.noBacklight();
            }
        }

        // Receber senha // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // 
        else {

            if (contador == 0) {
                digitada[contador] = tecla;
                lcd.backlight();
                lcd.clear();
                lcd.home(); // posiciona o texto do lcd no ponto incial
                lcd.print("Digite Senha");
                lcd.setCursor(5, 1);
                lcd.print("*");

            }
            else {
                digitada[contador] = tecla;
                lcd.print("*");
            }

            Serial.print("\nTecla: ");
            Serial.print(tecla);
            contador = contador + 1;
            Serial.print("\nContador: ");
            Serial.print(contador);
        }
    }
}

int menuADM(int armazenadas) {

    // Mensagem do menu
    lcd.clear();
    lcd.home(); // Posiciona o texto do lcd no ponto incial
    lcd.print("AddDelEdtResRet");
    lcd.setCursor(0, 1);
    lcd.print("(1)(3)(7)(9)(0)");
    Serial.print("\nMenu ADM");
    //lcd.setCursor(0,1);
    Serial.print("\nAdd(1) Del(3) Edt(7) Res(9) Voltar(0)");
    tecla = teclado.getKey();
    Serial.print("\nTecla: ");
    Serial.print(tecla);

    // Entradas inválidas
    while (tecla != '1' and tecla != '3' and tecla != '7' and tecla != '9' and tecla != '0') {

        //Serial.print("\nTecla inválida");
        tecla = teclado.getKey();
    }

    if (tecla == '0') {

        lcd.noBacklight();
        return armazenadas;
    }


    // Adicionar senha // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- //
    else if (tecla == '1') {

        lcd.clear();
        lcd.home(); // Posiciona o texto do lcd no ponto incial
        lcd.print("Nova senha");
        lcd.setCursor(5, 1); // Posiciona na linha 1 para aparecer a senha digitada
        Serial.print("\nNova senha\n");

        i = 0;
        tecla = teclado.getKey();
        while (i < tamanhoSenha) {
            tecla = teclado.getKey();

            // Digitou algo
            if (tecla != NO_KEY) {

                if (dado[0].posicaoDel == 0) {

                    dado[armazenadas + 1].senhas[i] = tecla;
                }
                else {

                    dado[dado[0].posicaoDel].senhas[i] = tecla;
                }
                i = i + 1;
                // Print do caracter *
                lcd.print("*");
                Serial.print("*");
            }
        }
        if (dado[0].posicaoDel != 0) {

            lcd.clear();
            lcd.home(); // Posiciona o texto do lcd no ponto incial
            lcd.print("Senha");
            lcd.setCursor(4, 1);
            lcd.print("adicionada");
            Serial.print("\nSenha adicionada com sucesso");

            Serial.print(dado[dado[0].posicaoDel].senhas);
            dado[0].posicaoDel = 0; // Caso tenha salvado senha em uma posição deletada, remove essa posição da memória
            return armazenadas;

        }
        else {

            lcd.clear();
            lcd.home(); // Posiciona o texto do lcd no ponto incial
            lcd.print("Senha");
            lcd.setCursor(4, 1);
            lcd.print("adicionada");

            Serial.print("\nSenha adicionada com sucesso");
            Serial.print(dado[armazenadas + 1].senhas);

            return armazenadas + 1;
        }
    }

    // Excluir senha // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- //
    else if (tecla == '3') {

        lcd.clear();
        lcd.home(); // Posiciona o texto do lcd no ponto incial
        lcd.print("Digite senha");
        lcd.setCursor(0, 1);
        lcd.print("del");
        Serial.print("\nDigite senha del\n");
        lcd.setCursor(5, 1); // Posiciona na linha 1 para aparecer a senha digitada

        // Recebe senha a ser deletada
        i = 0;
        while (i < tamanhoSenha) {
            tecla = teclado.getKey();

            // Digitou algo
            if (tecla != NO_KEY) {

                digitada[i] = tecla;
                lcd.print(tecla);
                i = i + 1;
            }
        }

        // Verifica se a senha a ser deletada existe
        Serial.print("\nVerificando senha");
        encontrou = verificar(digitada);

        // Senha encontrada
        if (encontrou == true and posicao(digitada) != 0) {

            contador = posicao(digitada);
            dado[0].posicaoDel = contador;
            // Senha armazenada na posição da apagada passa a ser igual a adm
            for (i = 0; i < tamanhoSenha; i++) {
                dado[contador].senhas[i] = dado[0].senhaADM[i];
            }
            lcd.clear();
            lcd.home(); // Posiciona o texto do lcd no ponto incial
            lcd.print("Senha apagada");

            delay(3000); // Espera 3 segundos e apaga o lcd
            lcd.noBacklight();
            Serial.print("\nSenha apagada");
        }

        // Senha não encontrada
        else {
            lcd.clear();
            lcd.home(); // Posiciona o texto do lcd no ponto incial
            lcd.print("Senha nao");
            lcd.setCursor(4, 1);
            lcd.print("encontrada");
            Serial.print("\nSenha não encontrada");
        }
        return armazenadas;
    }


    // Editar Senha // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- //
    else if (tecla == '7') {

        lcd.clear();
        lcd.home(); // Posiciona o texto do lcd no ponto incial
        lcd.print("Digite senha");
        lcd.setCursor(0, 1);
        lcd.print("edt");
        Serial.print("\nDigite senha editar\n");
        lcd.setCursor(5, 1); // Posiciona na linha 1 para aparecer a senha digitada

        // Recebe senha a ser editada
        i = 0;
        while (i < tamanhoSenha) {
            tecla = teclado.getKey();

            // Digitou algo
            if (tecla != NO_KEY) {

                digitada[i] = tecla;
                lcd.print(tecla);
                i = i + 1;
            }
        }
        // Verifica se a senha a ser editada existe
        Serial.print("\nVerificando senha");
        encontrou = verificar(digitada);

        // Senha encontrada
        if (encontrou == true and posicao(digitada) != 0) {

            lcd.clear();
            lcd.home(); // Posiciona o texto do lcd no ponto incial
            lcd.print("Nova senha:");
            Serial.print("\nSenha anterior: ");
            Serial.print(dado[contador].senhas);
            lcd.setCursor(5, 1); // Lcd posicionado no inicio da segunda linha
            Serial.print("\nNova senha: ");

            contador = posicao(digitada);
            // Recebe a nova senha editada
            i = 0;
            while (i < tamanhoSenha) {
                tecla = teclado.getKey();

                // Digitou algo
                if (tecla != NO_KEY) {

                    lcd.print("*");
                    Serial.print("*");
                    dado[contador].senhas[i] = tecla;

                    i = i + 1;
                }
            }
            lcd.clear();
            lcd.home(); // Posiciona o texto do lcd no ponto incial
            lcd.print("Senha");
            lcd.setCursor(2, 1);
            lcd.print("alterada");
        }

        // Senha não encontrada
        else {
            lcd.clear();
            lcd.home(); // Posiciona o texto do lcd no ponto incial
            lcd.print("Senha nao");
            lcd.setCursor(4, 1);
            lcd.print("encontrada");


            Serial.print("\nSenha não encontrada");
        }
        return armazenadas;
    }

    // Resetar fechadura // ---------- // ---------- // ---------- // ---------- // ---------- // ---------- //
    else if (tecla == '9') {

        lcd.clear();
        lcd.home(); // Posiciona o texto do lcd no ponto incial
        lcd.print("Resetar");
        lcd.setCursor(0, 1);
        lcd.print("Sim(A) Sair(0)");
        Serial.print("\nResetar fechadura");
        Serial.print("Digite A para avançar ou 0 para sair");

        // Entradas inválidas
        while (tecla != 'A' and tecla != '0') {

            //Serial.print("\nTecla inválida");
            tecla = teclado.getKey();
        }

        if (tecla == '0') {

            lcd.noBacklight();
            return armazenadas;
        }
        else if (tecla == 'A') {

            lcd.clear();
            lcd.home(); // posiciona o texto do lcd no ponto incial
            lcd.print("Senha ADM");
            Serial.print("\nDigite a senha de Administrador");
            ADM = verificarADM();

            if (ADM == true) {

                contador = 1; // Começar por dado[1] e manter a senha de adm
                while (contador <= armazenadas) {

                    for (i = 0; i < tamanhoSenha; i++) {
                        dado[contador].senhas[i] = dado[0].senhaADM[i];
                    }
                    lcd.print("*");
                    contador = contador + 1;
                }

                lcd.clear();
                lcd.home(); // posiciona o texto do lcd no ponto incial
                lcd.print("Fechadura");
                lcd.setCursor(2, 1);
                lcd.print("Resetada");
                Serial.print("\nFechadura fechada");
                Serial.print("\nFechaduraResetada");
                armazenadas = 1;
                return armazenadas;
            }
            else {
                lcd.clear();
                lcd.home(); // posiciona o texto do lcd no ponto incial
                lcd.print("Senha ADM");
                lcd.setCursor(2, 1);
                lcd.print("Incorreta");
                Serial.print("\nSenha de Administrador incorreta");
            }
        }
    }
}

// ---------- // ---------- // ---------- // ---------- // ---------- // ---------- //// ---------- // ---------- // ---------- // ---------- // ---------- // ---------- //
                                                                          // FUNÇÕES

                                // Indica se está armazenado na struct
bool verificar(char entrada[tamanhoSenha + 1]) {

    encontrou = false;
    contador = -1; // Iniciar análise pelo dado[0]
    while (contador <= armazenadas and encontrou == false) {

        i = 0;
        contador = contador + 1; // Próxima posição do struct

        // Verificar se a senha está armazenada    
        while (entrada[i] == dado[contador].senhas[i] and i < tamanhoSenha - 1) { // i só vai até tamanhosenha-1 pois são os valores que importam no char, caso tenha escrito mais que isso, senha já é inválida

            i = i + 1;

            if (entrada[i] == dado[contador].senhas[i] and i == tamanhoSenha - 1) {

                encontrou = true;
            }
        }
    }
    return encontrou;
}

// ---------- // ---------- // ---------- // ---------- // ---------- // ---------- //
                                // Recebe e verifica senha do ADM
bool verificarADM() {

    lcd.setCursor(5, 1);
    ADM = false; // Para verificar a senha
    i = 0;
    tecla = teclado.getKey();
    while (i < tamanhoSenha) {
        tecla = teclado.getKey();

        // Digitou algo
        if (tecla != NO_KEY) {

            digitada[i] = tecla;

            i = i + 1;

            // Print do caracter *

            lcd.print("*");
            Serial.print("*");
        }
    }

    // Verificar se a senha está armazenada
    i = 0;
    while (digitada[i] == dado[0].senhaADM[i] and i < tamanhoSenha - 1) {

        i = i + 1;

        if (digitada[i] == dado[0].senhaADM[i] and i == tamanhoSenha - 1) {

            ADM = true;
        }
    }

    return ADM;
}

// ---------- // ---------- // ---------- // ---------- // ---------- // ---------- //
                                // Inidica a posição no struct 
int posicao(char entrada[tamanhoSenha + 1]) {

    encontrou = false;
    contador = -1;
    while (contador <= armazenadas and encontrou == false) {

        i = 0;
        contador = contador + 1; // Próxima posição do struct

        // Verificar se a senha está armazenada    
        while (entrada[i] == dado[contador].senhas[i] and i < tamanhoSenha - 1) {

            i = i + 1;

            if (entrada[i] == dado[contador].senhas[i] and i == tamanhoSenha - 1) {

                encontrou = true;
            }
        }
    }
    return contador;
}