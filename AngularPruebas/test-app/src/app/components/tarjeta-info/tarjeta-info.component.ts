import { Component, Input } from '@angular/core';

@Component({
  selector: 'app-tarjeta-info',
  templateUrl: './tarjeta-info.component.html',
  styleUrls: ['./tarjeta-info.component.css']
})
export class TarjetaInfoComponent {
  @Input() titulo: string = "Aquí va el título";
  @Input() contenido: string = "Aquí va el contenido";
}
