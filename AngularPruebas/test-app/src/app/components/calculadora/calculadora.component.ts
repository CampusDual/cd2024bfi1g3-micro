import { Component } from '@angular/core';
import { EventEmitter, Output } from '@angular/core';

@Component({
  selector: 'app-calculadora',
  templateUrl: './calculadora.component.html',
  styleUrls: ['./calculadora.component.css']
})
export class CalculadoraComponent {
  @Output() moduleChange = new EventEmitter<string>();
  changeModule(module: string) {
    this.moduleChange.emit(module);
  }

  display: string = '';  // Mostrar la operación completa en display
  previousInput: string = '';   // Número anterior
  currentInput: string = '';    // Número actual
  operator: string = '';        // Operador actual
  waitingForResult: boolean = false;  // Indica si estamos esperando el resultado

  addNumber(number: string): void {
    if (this.waitingForResult) {
      this.currentInput = number;  // Reinicia el número actual después de un cálculo
      this.waitingForResult = false;
    } else {
      this.currentInput += number;  // Añade el número a la entrada actual
    }
    this.display = this.previousInput + this.operator + this.currentInput;  // Muestra la operación en el display
  }

  addOperator(op: string): void {
    if (this.currentInput === '') return;  // No se puede añadir operador sin número actual
    if (this.previousInput !== '' && !this.waitingForResult) {
      this.calculateResult();  // Calcula el resultado si ya hay una operación previa
    }
    this.operator = op;
    this.previousInput = this.currentInput;
    this.currentInput = '';  // Limpia la entrada actual para el próximo número
    this.display = this.previousInput + this.operator;  // Muestra la operación en el display
  }

  addDecimal(): void {
    if (this.currentInput.includes('.')) return;  // Evita añadir más de un punto decimal
    this.currentInput += '.';
    this.display = this.previousInput + this.operator + this.currentInput;  // Muestra la operación con el decimal
  }

  calculateResult(): void {
    if (this.previousInput === '' || this.currentInput === '') return;
    let result: number;
    const prev = parseFloat(this.previousInput);
    const current = parseFloat(this.currentInput);

    switch (this.operator) {
      case '+':
        result = prev + current;
        break;
      case '-':
        result = prev - current;
        break;
      case '*':
        result = prev * current;
        break;
      case '/':
        if (current === 0) {
          result = NaN;  // Error por división entre 0
        } else {
          result = prev / current;
        }
        break;
      default:
        return;
    }

    this.currentInput = result.toString();
    this.operator = '';
    this.previousInput = '';
    this.display = this.currentInput;  // Muestra solo el resultado
    this.waitingForResult = true;  // Indica que ya se calculó un resultado
  }

  clear(): void {
    this.currentInput = '';
    this.previousInput = '';
    this.operator = '';
    this.display = '';
    this.waitingForResult = false;  // Reinicia la calculadora
  }
}
