import { Component } from '@angular/core';

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.css']
})
export class HomeComponent {
  // Controlar si el sidenav está abierto
  sidenavOpened: boolean = true; // Por defecto abierto

  // Variables que controlan qué módulo se muestra en cada lado
  leftModule: string = 'tres-en-raya'; // Módulo izquierdo por defecto
  rightModule: string = 'calc';        // Módulo derecho por defecto

  // Cambia el módulo del lado izquierdo
  changeLeftModule(module: string) {
    this.leftModule = module;
  }

  // Cambia el módulo del lado derecho
  changeRightModule(module: string) {
    this.rightModule = module;
  }
}
