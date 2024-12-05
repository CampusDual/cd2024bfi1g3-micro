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
  display: string = '';        
  previousInput: string = ''; 
  currentInput: string = '';  
  operator: string = '';      

  addNumber(number: string): void {
    this.currentInput += number;
  }

  addOperator(op: string): void {
    if (this.currentInput === '') return;
    if (this.previousInput !== '') {
      this.calculateResult(); 
    }
    this.operator = op;
    this.previousInput = this.currentInput;
    this.currentInput = '';
  }

  addDecimal(): void {
    if (this.currentInput.includes('.')) return;
    this.currentInput += '.';
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
          result = NaN; 
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
  }

  clear(): void {
    this.currentInput = '';
    this.previousInput = '';
    this.operator = '';
  }
}
