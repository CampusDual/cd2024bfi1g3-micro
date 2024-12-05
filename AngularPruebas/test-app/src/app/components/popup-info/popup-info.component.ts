import { Component, Inject, Input } from '@angular/core';
import { MAT_DIALOG_DATA } from '@angular/material/dialog';

@Component({
  selector: 'app-popup-info',
  templateUrl: './popup-info.component.html',
  styleUrls: ['./popup-info.component.css']
})
export class PopupInfoComponent {

  constructor(@Inject(MAT_DIALOG_DATA) public data: any){}
  
}
