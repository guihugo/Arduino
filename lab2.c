// ULA 4 BITS COMPLETA (SOMA, SUBTRAÇÃO, ROTAÇÕES E CARRY)
// Hardware: 5 Botões + 5 LEDs

// --- PINOS DOS BOTÕES ---
const int btn_Esq   = 2; // Rotação Esquerda
const int btn_Dir   = 3; // Rotação Direita
const int btn_Sub   = 4; // SUBTRATOR (-1) 
const int btn_Reset = 5; // Reset
const int btn_Soma  = 6; // SOMADOR (+1)

// --- PINOS DOS LEDS ---
const int led_Carry = 8;  // LED de Carry/Borrow
const int led_Bit3  = 13; // MSB (8)
const int led_Bit2  = 12; // (4)
const int led_Bit1  = 11; // (2)
const int led_Bit0  = 10; // LSB (1)

// --- VARIÁVEIS DO SISTEMA ---
byte valor = 0;          
bool carry_flag = false; 

// --- MÁSCARAS BITWISE ---
const byte MASK_4BITS = 0x0F;
const byte MASK_MSB   = 0x08; 
const byte MASK_LSB   = 0x01; 

void setup() {
  Serial.begin(9600);
  
  // Configura Botões com Pull-up (Apertado = LOW)
  pinMode(btn_Esq, INPUT_PULLUP);
  pinMode(btn_Dir, INPUT_PULLUP);
  pinMode(btn_Sub, INPUT_PULLUP);
  pinMode(btn_Reset, INPUT_PULLUP);
  pinMode(btn_Soma, INPUT_PULLUP);
  
  // Configura LEDs
  pinMode(led_Bit3, OUTPUT);
  pinMode(led_Bit2, OUTPUT);
  pinMode(led_Bit1, OUTPUT);
  pinMode(led_Bit0, OUTPUT);
  pinMode(led_Carry, OUTPUT);

  Serial.println("=== ULA 5 BOTOES INICIADA ===");
  Serial.println("Btn 6: Somar | Btn 4: Subtrair");
  atualizarHardware();
}

void loop() {
  
  // --- 1. SOMADOR (+1) - Porta 6 ---
  if (digitalRead(btn_Soma) == LOW) {
    valor++;
    
    // Verifica Estouro (Overflow)
    if (valor > 15) { 
      valor = 0;         
      carry_flag = true; )
      Serial.println("++ Soma: Estourou (Carry ON)");
    } else {
      carry_flag = false;
      Serial.println("++ Soma (+1)");
    }
    
    atualizarHardware();
    esperarSoltar(btn_Soma);
  }

  // --- 2. SUBTRATOR (-1) - Porta 4 ---
  if (digitalRead(btn_Sub) == LOW) {
    // Verifica "Estouro Negativo" (Underflow)
    if (valor == 0) {
      valor = 15;        
      carry_flag = true; 
      Serial.println("-- Subtracao: Underflow (Borrow ON)");
    } else {
      valor--;
      carry_flag = false;
      Serial.println("-- Subtracao (-1)");
    }
    
    atualizarHardware();
    esperarSoltar(btn_Sub);
  }

  // --- 3. ROTAÇÃO ESQUERDA (com Carry) - Porta 2 ---
  if (digitalRead(btn_Esq) == LOW) {
    bool novoCarry = (valor & MASK_MSB) != 0; 
    byte valorDeslocado = (valor << 1) & MASK_4BITS;
    
    if (carry_flag) {
      valor = valorDeslocado | MASK_LSB;
    } else {
      valor = valorDeslocado;
    }
    
    carry_flag = novoCarry;
    Serial.println("<< Rotacao Esquerda");
    atualizarHardware();
    esperarSoltar(btn_Esq);
  }

  // --- 4. ROTAÇÃO DIREITA (com Carry) - Porta 3 ---
  if (digitalRead(btn_Dir) == LOW) {
    bool novoCarry = (valor & MASK_LSB) != 0; // O LSB vira o novo Carry
    byte valorDeslocado = (valor >> 1);
    
    if (carry_flag) {
      valor = valorDeslocado | MASK_MSB;
    } else {
      valor = valorDeslocado;
    }
    
    carry_flag = novoCarry;
    Serial.println(">> Rotacao Direita");
    atualizarHardware();
    esperarSoltar(btn_Dir);
  }

  // --- 5. RESET - Porta 5 ---
  if (digitalRead(btn_Reset) == LOW) {
    valor = 0;
    carry_flag = false;
    Serial.println("!! RESET !!");
    atualizarHardware();
    esperarSoltar(btn_Reset);
  }
}

// --- FUNÇÕES AUXILIARES ---

void atualizarHardware() {
  digitalWrite(led_Bit3, (valor & 8) ? HIGH : LOW);
  digitalWrite(led_Bit2, (valor & 4) ? HIGH : LOW);
  digitalWrite(led_Bit1, (valor & 2) ? HIGH : LOW);
  digitalWrite(led_Bit0, (valor & 1) ? HIGH : LOW);
  
  digitalWrite(led_Carry, carry_flag ? HIGH : LOW);
}

void esperarSoltar(int pino) {
  delay(50); // Filtro (Debounce)
  while(digitalRead(pino) == LOW); 
  delay(50);
}
