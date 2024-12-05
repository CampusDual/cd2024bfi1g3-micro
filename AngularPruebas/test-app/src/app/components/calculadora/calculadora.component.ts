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
  waitingForResult: boolean = false; 

  addNumber(number: string): void {
    if (this.waitingForResult) {
      this.currentInput = number;  
      this.waitingForResult = false;
    } else {
      this.currentInput += number;  
    }
    this.display = this.previousInput + this.operator + this.currentInput; 
  }

  addOperator(op: string): void {
    if (this.currentInput === '') return;  
    if (this.previousInput !== '' && !this.waitingForResult) {
      this.calculateResult();  
    }
    this.operator = op;
    this.previousInput = this.currentInput;
    this.currentInput = ''; 
    this.display = this.previousInput + this.operator;  
  }

  addDecimal(): void {
    if (this.currentInput.includes('.')) return;  
    this.currentInput += '.';
    this.display = this.previousInput + this.operator + this.currentInput;  
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
    if(this.currentInput==="NaN"){
      this.currentInput= "Math Error"
    }
    this.operator = '';
    this.previousInput = '';
    this.display = this.currentInput;
    this.waitingForResult = true; 
  }

  clear(): void {
    this.currentInput = '';
    this.previousInput = '';
    this.operator = '';
    this.display = '';
    this.waitingForResult = false; 
  }
}
