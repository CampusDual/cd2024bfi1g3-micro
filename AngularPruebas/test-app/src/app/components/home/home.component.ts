import { Component } from '@angular/core';

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.css']
})
export class HomeComponent {
  // Variables para manejar los módulos
  leftModule: string = 'tres-en-raya';  // Módulo izquierdo por defecto
  rightModule: string = 'calc';         // Módulo derecho por defecto

  // Cambia el módulo del lado izquierdo
  changeLeftModule(module: string) {
    this.leftModule = module;
  }

  // Cambia el módulo del lado derecho
  changeRightModule(module: string) {
    this.rightModule = module;
  }
}
