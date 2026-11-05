# Sistema-de-Sem-foro-Urbano-com-Arduino

Projeto acadêmico de um sistema de semáforo inteligente utilizando Arduino Uno, desenvolvido para simulação interativa no Wokwi.

---

## Como Testar a Simulação Online

Você pode testar o funcionamento do semáforo em tempo real diretamente no seu navegador:

https://wokwi.com/projects/479867504886893569

### Instruções para teste:
1. Clique no botão de Play (triângulo verde no Wokwi).
2. Clique no botão azul para solicitar a travessia do pedestre.
3. Acompanhe a sequência dos LEDs, o visor LCD e os sinais sonoros.

---

## Regras de Funcionamento

* **Sinal Verde (Carros):** Estado padrão com trânsito de veículos liberado.
* **Solicitação de Travessia:** O pedestre pressiona o botão de chamada.
* **Fase de Preparo (30s):** O display exibe contagem regressiva de segurança.
* **Fase de Amarelo (30s):** Alerta de parada para os motoristas.
* **Travessia de Pedestres (40s):** Sinal verde para pedestres por 30s + 10s de alerta piscante com sinal sonoro no buzzer.
* **Cooldown de Carros (30s):** Intervalo mínimo de trânsito livre obrigatório para veículos antes de aceitar uma nova solicitação.

---

## Componentes do Circuito

* 1x Arduino Uno
* 3x LEDs para Veículos (Vermelho - Pino 12, Amarelo - Pino 11, Verde - Pino 10)
* 2x LEDs para Pedestres (Vermelho - Pino 7, Verde - Pino 6)
* 1x Botão Pushbutton (Pino 2)
* 1x Buzzer Piezoelétrico (Pino 4)
* 1x Display LCD 16x2 I2C (Endereço 0x27 - Pinos SDA A4 / SCL A5)
