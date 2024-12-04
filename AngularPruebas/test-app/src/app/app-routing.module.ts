import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { TresRayaComponent } from './tres-raya/tres-raya.component';
import { CalculadoraComponent } from './calculadora/calculadora.component';

const routes: Routes = [
  {path: 'tres-raya', component: TresRayaComponent},
  {path: 'calc', component:CalculadoraComponent}
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
