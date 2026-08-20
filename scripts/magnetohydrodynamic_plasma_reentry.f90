module mhd_plasma_physics
  implicit none
  double precision, parameter :: e_charge = 1.60217663d-19 ! C
  double precision, parameter :: m_e = 9.10938370d-31      ! kg
  double precision, parameter :: eps0 = 8.85418781d-12     ! F/m

contains

  subroutine compute_plasma_frequency(electron_density, omega_p)
    double precision, intent(in)  :: electron_density ! electrons/m^3
    double precision, intent(out) :: omega_p          ! rad/s
    
    ! Plasma Angular Frequency: w_p = sqrt((n_e * e^2) / (m_e * eps0))
    omega_p = sqrt((electron_density * (e_charge**2)) / (m_e * eps0))
  end subroutine compute_plasma_frequency

end module mhd_plasma_physics

program test_mhd
  use mhd_plasma_physics
  implicit none
  double precision :: n_e, w_p
  
  n_e = 1.0d18 ! High hypersonic shock layer electron density
  call compute_plasma_frequency(n_e, w_p)
  print *, 'Hypersonic Shock Plasma Frequency (rad/s):', w_p
end program test_mhd
