// source tab dimensions
s_x = 25.0;
s_y = 10.0;
s_z = 3.3;
hole_r = 1.6;
hole_w = 18.5;
chamfer = 2.0;

// body dimensions
b_x = 11.0;
b_y = 9.6;
b_z = 6.2-3.3;

// the drain and gate conductors
c_x = 5.0;
c_y = (17.0-b_y)/2;
c_z = 0.1;

d = 0.1;
// The source tab
difference() {
    // The source tab outline
    color("blue") translate([-s_x/2,-s_y/2,0]) cube([s_x, s_y, s_z]);
    // the hole cutouts
    translate([-hole_w/2,0,-d]) cylinder(s_z+2*d,hole_r,hole_r);
    translate([+hole_w/2,0,-d]) cylinder(s_z+2*d,hole_r,hole_r);
    translate([-s_x/2-d,-hole_r,-d]) cube([(s_x-hole_w)/2+d,2*hole_r,s_z+2*d]);
    translate([s_x/2-(s_x-hole_w)/2,-hole_r,-d]) cube([(s_x-hole_w)/2+d,2*hole_r,s_z+2*d]);
    // the chamfers
    translate([-s_x/2,s_y/2-chamfer/sqrt(2),-d]) rotate([0,0,45]) cube([chamfer,chamfer,s_z+2*d]);
    translate([s_x/2,s_y/2-chamfer/sqrt(2),-d]) rotate([0,0,45]) cube([chamfer,chamfer,s_z+2*d]);
    translate([-s_x/2,-s_y/2-chamfer/sqrt(2),-d]) rotate([0,0,45]) cube([chamfer,chamfer,s_z+2*d]);
    translate([s_x/2,-s_y/2-chamfer/sqrt(2),-d]) rotate([0,0,45]) cube([chamfer,chamfer,s_z+2*d]);
}

// The transistor body
color("black") translate([-b_x/2,-b_y/2,s_z]) cube([b_x,b_y,b_z]);
// The drain conductor
difference() {
    color("gray") translate([-c_x/2,b_y/2,4.5]) cube([c_x,c_y,c_z]);
    // chamfer
    translate([-c_x/2,b_y/2+c_y-chamfer/sqrt(2),s_z-d]) rotate([0,0,45]) cube([chamfer,chamfer,s_z+2*d]);
}

// The gate conductor
color("gray") translate([-c_x/2,-b_y/2-c_y,4.5]) cube([c_x,c_y,c_z]);

