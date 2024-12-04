import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { TresRayaComponent } from './components/tres-raya/tres-raya.component';
import { CalculadoraComponent } from './components/calculadora/calculadora.component';
import { HomeComponent } from './components/home/home.component';

const routes: Routes = [
  {path: 'tres-raya', component: TresRayaComponent},
  {path: 'calc', component:CalculadoraComponent},
  {path: 'home', component:HomeComponent},
  {path: '', redirectTo: '/home', pathMatch: 'full' },
  
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }



