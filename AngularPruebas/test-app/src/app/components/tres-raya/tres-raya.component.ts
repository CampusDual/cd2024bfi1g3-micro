import { Component } from '@angular/core';
import { EventEmitter, Output } from '@angular/core';

@Component({
  selector: 'app-tres-raya',
  templateUrl: './tres-raya.component.html',
  styleUrls: ['./tres-raya.component.css'],
})
export class TresRayaComponent {
  @Output() moduleChange = new EventEmitter<string>();
  changeModule(module: string) {
    this.moduleChange.emit(module);
  }
  posiciones = [
    [' ', ' ', ' '],
    [' ', ' ', ' '],
    [' ', ' ', ' '],
  ];

  jugadorActual = 'X';
  marcadorX: number = 0;
  marcadorO: number = 0;

  presion(fila: number, columna: number) {
    if (this.casillaLibre(fila, columna)) {
      this.posiciones[fila][columna] = this.jugadorActual;
      this.verificarGano('X');
      this.verificarGano('O');
      this.cambiarJugador();
    }
  }

  casillaLibre(fila: number, columna: number): boolean {
    return this.posiciones[fila][columna] == ' ';
  }

  verificarGano(jugador: string) {
    if (
      this.posiciones[0][0] == jugador &&
      this.posiciones[0][1] == jugador &&
      this.posiciones[0][2] == jugador
    ) {
      this.anunciarGanador(jugador);
    }

    if (
      this.posiciones[1][0] == jugador &&
      this.posiciones[1][1] == jugador &&
      this.posiciones[1][2] == jugador
    ) {
      this.anunciarGanador(jugador);
    }

    if (
      this.posiciones[2][0] == jugador &&
      this.posiciones[2][1] == jugador &&
      this.posiciones[2][2] == jugador
    ) {
      this.anunciarGanador(jugador);
    }

    if (
      this.posiciones[0][0] == jugador &&
      this.posiciones[1][0] == jugador &&
      this.posiciones[2][0] == jugador
    ) {
      this.anunciarGanador(jugador);
    }

    if (
      this.posiciones[0][1] == jugador &&
      this.posiciones[1][1] == jugador &&
      this.posiciones[2][1] == jugador
    ) {
      this.anunciarGanador(jugador);
    }

    if (
      this.posiciones[0][2] == jugador &&
      this.posiciones[1][2] == jugador &&
      this.posiciones[2][2] == jugador
    ) {
      this.anunciarGanador(jugador);
    }

    if (
      this.posiciones[0][0] == jugador &&
      this.posiciones[1][1] == jugador &&
      this.posiciones[2][2] == jugador
    ) {
      this.anunciarGanador(jugador);
    }

    if (
      this.posiciones[0][2] == jugador &&
      this.posiciones[1][1] == jugador &&
      this.posiciones[2][0] == jugador
    ) {
      this.anunciarGanador(jugador);
    }
  }

  cambiarJugador() {
    if (this.jugadorActual == 'O') {
      this.jugadorActual = 'X';
    } else {
      this.jugadorActual = 'O';
    }
  }

  reiniciar() {
    for (let fila = 0; fila < this.posiciones.length; fila++) {
      for (let columna = 0; columna < this.posiciones.length; columna++) {
        this.posiciones[fila][columna] = ' ';
      }
    }
    this.jugadorActual = 'X';
  }

  actualizarMarcador(jugador: string) {
    if (jugador == 'O') {
      this.marcadorO++;
    } else {
      this.marcadorX++;
    }
  }

  anunciarGanador(jugador: string) {
    this.actualizarMarcador(jugador);
    alert(`Ganó: ${jugador}`);
    this.jugadorActual = 'X';
  }
}
