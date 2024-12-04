import { Component } from '@angular/core';

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.css']
})
export class HomeComponent {
  videoUrl: string = 'https://www.youtube.com/embed/dQw4w9WgXcQ'; // Cambia el enlace por el de tu video
}