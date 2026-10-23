options gen2
require ue_math
require math

// The plugin's native module ue_math (DasNativeMath.cpp). Expected values follow the engine formulas. main returns 0
// when every check passes, otherwise the number of the first failed check.
def same(a, b : double) : bool {
    return abs(a - b) < 0.0001lf
}
def near(a, b : UEVector) : bool {
    return ue_nearly_equal(a, b, 0.0001lf)
}

[export]
def main(input:int):int {
    // Interpolation: alpha = clamp(dt * speed, 0, 1) = 0.5; the constant step is speed * dt.
    if (!same(f_interp_to(0.0lf, 10.0lf, 0.1lf, 5.0lf), 5.0lf)) { return 1 }
    if (!same(f_interp_constant_to(0.0lf, 10.0lf, 0.1lf, 5.0lf), 0.5lf)) { return 2 }
    let x10 = ue_vector(10.0lf, 0.0lf, 0.0lf)
    if (!near(v_interp_to(zero_vector(), x10, 0.1lf, 5.0lf), ue_vector(5.0lf, 0.0lf, 0.0lf))) { return 3 }
    if (!near(v_interp_constant_to(zero_vector(), x10, 0.1lf, 5.0lf), ue_vector(0.5lf, 0.0lf, 0.0lf))) { return 4 }
    let turned = v_interp_normal_rotation_to(forward_vector(), right_vector(), 0.1lf, 90.0lf)
    if (!same(ue_length(turned), 1.0lf) || !(turned.y > 0.0lf)) { return 5 }
    let v2 = vector2d_interp_to(ue_vector2d(0.0lf, 0.0lf), ue_vector2d(0.0lf, 10.0lf), 0.1lf, 5.0lf)
    if (!ue_nearly_equal(v2, ue_vector2d(0.0lf, 5.0lf), 0.0001lf)) { return 6 }
    let v2c = vector2d_interp_constant_to(ue_vector2d(0.0lf, 0.0lf), ue_vector2d(0.0lf, 10.0lf), 1.0lf, 2.0lf)
    if (!ue_nearly_equal(v2c, ue_vector2d(0.0lf, 2.0lf), 0.0001lf)) { return 7 }
    let yaw90 = ue_rotator(0.0lf, 90.0lf, 0.0lf)
    if (!same(r_interp_to(zero_rotator(), yaw90, 0.1lf, 5.0lf).yaw, 45.0lf)) { return 8 }
    if (!same(r_interp_constant_to(zero_rotator(), yaw90, 1.0lf, 10.0lf).yaw, 10.0lf)) { return 9 }
    let q90 = yaw90.quaternion()
    if (!same(q_interp_to(ue_quat_identity(), q90, 0.1lf, 5.0lf).angular_distance(q90), DBL_PI / 4.0lf)) { return 10 }
    let grey = c_interp_to(ue_linear_color(0.0f, 0.0f, 0.0f, 1.0f), ue_linear_color(1.0f, 1.0f, 1.0f, 1.0f), 0.1lf, 5.0lf)
    if (!ue_nearly_equal(grey, ue_linear_color(0.5f, 0.5f, 0.5f, 1.0f), 0.0001lf)) { return 11 }
    // Easing.
    if (!same(ease_in(0.0lf, 10.0lf, 0.5lf, 2.0lf), 2.5lf) || !same(ease_out(0.0lf, 10.0lf, 0.5lf, 2.0lf), 7.5lf)) {
        return 12
    }
    if (!same(ease_in_out(0.0lf, 10.0lf, 0.5lf, 2.0lf), 5.0lf)) { return 13 }
    if (!same(sinusoidal_in(0.0lf, 10.0lf, 0.5lf), 10.0lf * (1.0lf - cos(DBL_PI / 4.0lf)))) { return 14 }
    if (!same(sinusoidal_out(0.0lf, 10.0lf, 0.5lf), 10.0lf * sin(DBL_PI / 4.0lf))) { return 15 }
    let mids = sinusoidal_in_out(0.0lf, 10.0lf, 0.5lf) + expo_in_out(0.0lf, 10.0lf, 0.5lf) + circular_in_out(0.0lf, 10.0lf, 0.5lf)
    if (!same(mids, 15.0lf)) { return 16 }
    if (!same(expo_in(0.0lf, 10.0lf, 1.0lf), 10.0lf) || !same(expo_out(0.0lf, 10.0lf, 0.0lf), 0.0lf)) { return 17 }
    if (!same(circular_in(0.0lf, 10.0lf, 1.0lf), 10.0lf) || !same(circular_out(0.0lf, 10.0lf, 0.0lf), 0.0lf)) {
        return 18
    }
    // Comparisons and ranges.
    if (!is_nearly_equal(1.0lf, 1.0lf + 1.0e-10lf) || !is_nearly_equal(1.0lf, 1.1lf, 0.2lf) || is_nearly_equal(1.0lf, 1.1lf, 0.01lf)) {
        return 19
    }
    if (!is_nearly_zero(1.0e-10lf) || is_nearly_zero(0.5lf, 0.1lf)) { return 20 }
    if (!is_within(5.0lf, 0.0lf, 10.0lf) || is_within(10.0lf, 0.0lf, 10.0lf) || !is_within_inclusive(10.0lf, 0.0lf, 10.0lf)) {
        return 21
    }
    let r0 = ue_vector2d(0.0lf, 10.0lf)
    let r1 = ue_vector2d(0.0lf, 100.0lf)
    if (!same(get_mapped_range_value_clamped(r0, r1, 5.0lf), 50.0lf) || !same(get_mapped_range_value_clamped(r0, r1, 20.0lf), 100.0lf)) {
        return 22
    }
    if (!same(get_mapped_range_value_unclamped(r0, r1, 20.0lf), 200.0lf) || !same(normalize_to_range(5.0lf, 0.0lf, 10.0lf), 0.5lf)) {
        return 23
    }
    // Angles and rounding.
    if (!same(find_delta_angle_degrees(350.0lf, 10.0lf), 20.0lf) || !same(unwind_degrees(370.0lf), 10.0lf)) { return 24 }
    if (!same(clamp_angle(45.0lf, -90.0lf, 90.0lf), 45.0lf) || !same(clamp_angle(100.0lf, -90.0lf, 90.0lf), 90.0lf)) {
        return 25
    }
    if (!same(degrees_to_radians(180.0lf), DBL_PI) || !same(radians_to_degrees(DBL_PI), 180.0lf)) { return 26 }
    if (!same(grid_snap(14.0lf, 10.0lf), 10.0lf) || !same(grid_snap(16.0lf, 10.0lf), 20.0lf)) { return 27 }
    if (!same(round_from_zero(-1.2lf), -2.0lf) || !same(round_from_zero(1.2lf), 2.0lf)) { return 28 }
    let pulse = make_pulsating_value(1.3lf, 2.0lf, 0.0lf)
    if (pulse < 0.0lf || pulse > 1.0lf) { return 29 }
    // Random values: ranges over many samples.
    var hit_low = false
    var hit_high = false
    for (i in range(0, 500)) {
        let n = rand_range(1, 6)
        if (n < 1 || n > 6) { return 30 }
        hit_low = hit_low || n == 1
        hit_high = hit_high || n == 6
        let d = rand_range(0.5lf, 1.5lf)
        let f = frand()
        if (d < 0.5lf || d > 1.5lf || f < 0.0lf || f >= 1.0lf) { return 31 }
        if (!same(ue_length(v_rand()), 1.0lf)) { return 32 }
        if (ue_dot(v_rand_cone(forward_vector(), 0.1lf), forward_vector()) < cos(0.1lf) - 0.0001lf) { return 33 }
        let c = rand_point_in_circle(2.0lf)
        if (ue_length(ue_vector(c.x, c.y, 0.0lf)) > 2.0001lf) { return 34 }
        let p = random_point_in_bounding_box(ue_vector(10.0lf, 0.0lf, 0.0lf), ue_vector(1.0lf, 2.0lf, 3.0lf))
        if (abs(p.x - 10.0lf) > 1.0lf || abs(p.y) > 2.0lf || abs(p.z) > 3.0lf) { return 35 }
        if (random_rotator(false).roll != 0.0lf) { return 36 }
        rand_bool()
    }
    if (!hit_low || !hit_high) { return 37 }
    // Geometry.
    let origin = zero_vector()
    if (!near(closest_point_on_line(origin, x10, ue_vector(5.0lf, 5.0lf, 0.0lf)), ue_vector(5.0lf, 0.0lf, 0.0lf))) {
        return 38
    }
    if (!near(closest_point_on_infinite_line(origin, x10, ue_vector(20.0lf, 5.0lf, 0.0lf)), ue_vector(20.0lf, 0.0lf, 0.0lf))) {
        return 39
    }
    if (!same(point_dist_to_segment(ue_vector(5.0lf, 5.0lf, 0.0lf), origin, x10), 5.0lf)) { return 40 }
    let hit = line_plane_intersection(ue_vector(0.0lf, 0.0lf, 10.0lf), ue_vector(0.0lf, 0.0lf, -10.0lf), origin, up_vector())
    if (!near(hit, origin)) { return 41 }
    let from = ue_vector(-10.0lf, 0.0lf, 0.0lf)
    if (!line_sphere_intersection(from, forward_vector(), 20.0lf, origin, 1.0lf)) { return 42 }
    if (line_sphere_intersection(from, up_vector(), 20.0lf, origin, 1.0lf)) { return 43 }
    // Noise: zero at integer points, in -1..1 elsewhere.
    if (!same(perlin_noise_1d(3.0lf), 0.0lf) || abs(perlin_noise_3d(ue_vector(0.3lf, 1.7lf, 2.2lf))) > 1.0lf) { return 44 }
    if (abs(perlin_noise_2d(ue_vector2d(0.3lf, 1.7lf))) > 1.0lf) { return 45 }

    // FVector: constants.
    if (zero_vector() != ue_vector(0.0lf, 0.0lf, 0.0lf) || one_vector() != ue_vector(1.0lf, 1.0lf, 1.0lf)) { return 46 }
    if (up_vector() != ue_vector(0.0lf, 0.0lf, 1.0lf) || down_vector() != ue_vector(0.0lf, 0.0lf, -1.0lf)) { return 47 }
    if (forward_vector() != ue_vector(1.0lf, 0.0lf, 0.0lf) || backward_vector() != ue_vector(-1.0lf, 0.0lf, 0.0lf)) {
        return 48
    }
    if (right_vector() != ue_vector(0.0lf, 1.0lf, 0.0lf) || left_vector() != ue_vector(0.0lf, -1.0lf, 0.0lf)) { return 49 }
    // FVector: checks and components.
    let v = ue_vector(3.0lf, -4.0lf, 12.0lf)
    if (!v.equals(v + ue_vector(0.00001lf, 0.0lf, 0.0lf)) || v.equals(v + one_vector(), 0.1lf)) { return 50 }
    if (!origin.is_zero() || v.is_zero() || !ue_vector(0.001lf, 0.0lf, 0.0lf).is_nearly_zero(0.01lf)) { return 51 }
    if (!up_vector().is_normalized() || !up_vector().is_unit(0.0001lf) || v.contains_nan()) { return 52 }
    if (v.get_abs() != ue_vector(3.0lf, 4.0lf, 12.0lf) || v.get_max() != 12.0lf || v.get_min() != -4.0lf) { return 53 }
    if (v.get_abs_max() != 12.0lf || v.component_min(origin) != ue_vector(0.0lf, -4.0lf, 0.0lf)) { return 54 }
    if (v.component_max(origin) != ue_vector(3.0lf, 0.0lf, 12.0lf) || v.get_sign_vector() != ue_vector(1.0lf, -1.0lf, 1.0lf)) {
        return 55
    }
    if (ue_vector(2.0lf, 4.0lf, 8.0lf).reciprocal() != ue_vector(0.5lf, 0.25lf, 0.125lf)) { return 56 }
    // FVector: sizes and distances.
    if (!same(v.size_2d(), 5.0lf) || !same(v.size_squared_2d(), 25.0lf)) { return 57 }
    if (!same(dist_2d(origin, ue_vector(3.0lf, 4.0lf, 100.0lf)), 5.0lf) || !same(dist_squared(origin, ue_vector(1.0lf, 2.0lf, 2.0lf)), 9.0lf)) {
        return 58
    }
    if (!near(ue_vector(3.0lf, 4.0lf, 9.0lf).get_safe_normal_2d(), ue_vector(0.6lf, 0.8lf, 0.0lf))) { return 59 }
    if (!near(ue_vector(30.0lf, 40.0lf, 0.0lf).get_clamped_to_max_size(5.0lf), ue_vector(3.0lf, 4.0lf, 0.0lf))) { return 60 }
    if (!near(ue_vector(3.0lf, 4.0lf, 0.0lf).get_clamped_to_size(10.0lf, 20.0lf), ue_vector(6.0lf, 8.0lf, 0.0lf))) {
        return 61
    }
    if (!near(v.grid_snap(5.0lf), ue_vector(5.0lf, -5.0lf, 10.0lf))) { return 62 }
    if (!points_are_near(v, v + ue_vector(0.5lf, 0.0lf, 0.0lf), 1.0lf)) { return 63 }
    // FVector: directions.
    if (!near(forward_vector().rotate_angle_axis(90.0lf, up_vector()), right_vector())) { return 64 }
    if (!near(ue_vector(1.0lf, -1.0lf, 0.0lf).mirror_by_vector(right_vector()), ue_vector(1.0lf, 1.0lf, 0.0lf))) {
        return 65
    }
    if (!same(right_vector().heading_angle(), DBL_PI / 2.0lf) || !same(forward_vector().cosine_angle_2d(right_vector()), 0.0lf)) {
        return 66
    }
    if (!same(right_vector().rotation().yaw, 90.0lf)) { return 67 }
    if (!near(right_vector().to_orientation_quat().get_forward_vector(), right_vector())) { return 68 }

    // FRotator.
    let r = ue_rotator(10.0lf, 20.0lf, 30.0lf)
    if (zero_rotator() != ue_rotator(0.0lf, 0.0lf, 0.0lf) || !zero_rotator().is_zero() || r.is_zero()) { return 69 }
    if (!r.equals(r) || r.equals(yaw90, 0.1lf) || !ue_rotator(0.001lf, 0.0lf, 0.0lf).is_nearly_zero(0.01lf)) { return 70 }
    if (!ue_nearly_equal(r.get_inverse().get_inverse(), r, 0.0001lf)) { return 71 }
    if (!same(ue_rotator(0.0lf, 370.0lf, 0.0lf).get_normalized().yaw, 10.0lf)) { return 72 }
    if (!same(ue_rotator(0.0lf, -10.0lf, 0.0lf).get_denormalized().yaw, 350.0lf)) { return 73 }
    if (!same(ue_rotator(0.0lf, -10.0lf, 0.0lf).get_clamped().yaw, 350.0lf)) { return 74 }
    if (!same(normalize_axis(190.0lf), -170.0lf) || !same(clamp_axis(-10.0lf), 350.0lf)) { return 75 }
    if (!near(yaw90.vector(), right_vector()) || !near(r.euler(), ue_vector(30.0lf, 10.0lf, 20.0lf))) { return 76 }
    if (!same(zero_rotator().get_manhattan_distance(r), 60.0lf)) { return 77 }
    if (!same(make_from_x(right_vector()).yaw, 90.0lf) || !make_from_xz(forward_vector(), up_vector()).equals(zero_rotator())) {
        return 78
    }
    // MakeFromY / MakeFromZ fix only the given axis; the engine leaves the other two unspecified.
    let from_y = make_from_y(right_vector()).quaternion().get_right_vector()
    if (!near(from_y, right_vector()) || !near(make_from_z(up_vector()).quaternion().get_up_vector(), up_vector())) {
        return 79
    }
    if (!make_from_xy(forward_vector(), right_vector()).equals(zero_rotator())) { return 80 }
    if (!make_from_yx(right_vector(), forward_vector()).equals(zero_rotator())) { return 80 }
    if (!make_from_yz(right_vector(), up_vector()).equals(zero_rotator())) { return 80 }
    if (!make_from_zx(up_vector(), forward_vector()).equals(zero_rotator())) { return 80 }
    if (!make_from_zy(up_vector(), right_vector()).equals(zero_rotator())) { return 80 }

    // FQuat.
    let identity = ue_quat_identity()
    if (!identity.is_identity() || q90.is_identity() || !q90.equals(q90) || q90.equals(identity, 0.1lf)) { return 81 }
    if (!same(q90.get_angle(), DBL_PI / 2.0lf) || !same(identity.angular_distance(q90), DBL_PI / 2.0lf)) { return 82 }
    if (!near(q90.get_forward_vector(), right_vector()) || !near(q90.get_axis_x(), right_vector())) { return 83 }
    if (!near(q90.get_right_vector(), backward_vector()) || !near(q90.get_axis_y(), backward_vector())) { return 84 }
    if (!near(q90.get_up_vector(), up_vector()) || !near(q90.get_axis_z(), up_vector())) { return 85 }
    if (!near(q90.get_rotation_axis(), up_vector()) || !near(q90.vector(), right_vector())) { return 86 }
    if (!same(q90.euler().z, 90.0lf) || !near(make_from_euler(ue_vector(0.0lf, 0.0lf, 90.0lf)).vector(), right_vector())) {
        return 87
    }
    if (!near(find_between_vectors(forward_vector(), right_vector()).vector(), right_vector())) { return 88 }
    if (!near(find_between_normals(forward_vector(), right_vector()).vector(), right_vector())) { return 89 }
    if (!same(slerp_full_path(identity, q90, 0.5lf).angular_distance(q90), DBL_PI / 4.0lf)) { return 90 }
    if (fast_lerp(identity, q90, 0.0lf).equals(q90)) { return 91 }

    // FTransform.
    let t = ue_transform(ue_vector(10.0lf, 0.0lf, 0.0lf), yaw90, ue_vector(2.0lf, 2.0lf, 2.0lf))
    let u = ue_transform_identity()
    if (!t.equals(t) || t.equals(u) || !t.equals_no_scale(ue_transform(ue_vector(10.0lf, 0.0lf, 0.0lf), yaw90, one_vector()), 0.0001lf)) {
        return 92
    }
    if (!t.get_relative_transform(t).equals(u, 0.0001lf)) { return 93 }
    if (!same(t.get_maximum_axis_scale(), 2.0lf) || !same(t.get_minimum_axis_scale(), 2.0lf)) { return 94 }
    if (!same(t.remove_scaling().get_maximum_axis_scale(), 1.0lf) || !same(t.get_determinant(), 8.0lf)) { return 95 }
    if (!near(t.transform_position_no_scale(forward_vector()), ue_vector(10.0lf, 1.0lf, 0.0lf))) { return 96 }
    if (!near(t.inverse_transform_position_no_scale(ue_vector(10.0lf, 1.0lf, 0.0lf)), forward_vector())) { return 97 }
    if (!t.inverse_transform_rotation(q90).is_identity() || !same(t.rotator().yaw, 90.0lf)) { return 98 }
    if (!blend(u, u, 0.5lf).equals(u)) { return 99 }

    // FVector2D.
    if (zero_vector2d() != ue_vector2d(0.0lf, 0.0lf)) { return 100 }
    if (!ue_vector2d(3.0lf, 4.0lf).get_safe_normal().equals(ue_vector2d(0.6lf, 0.8lf), 0.0001lf)) { return 101 }
    if (!ue_vector2d(1.0lf, 0.0lf).get_rotated(90.0lf).equals(ue_vector2d(0.0lf, 1.0lf), 0.0001lf)) { return 102 }
    return 0
}

// The call cost benchmark (interop-performance): f_interp_to through ue_math against the same formula in
// daScript (FMath::FInterpTo).
def interp_in_script(current, target, dt, speed : double) : double {
    if (speed <= 0.0lf) { return target }
    let dist = target - current
    if (dist * dist < 1.0e-8lf) { return target }
    return current + dist * clamp(dt * speed, 0.0lf, 1.0lf)
}

[export]
def bench_interp_native(count:int):int {
    var n = 0
    for (i in range(0, count)) {
        if (f_interp_to(double(i), 100.0lf, 0.016lf, 1.0lf) >= 0.0lf) { n += 1 }
    }
    return n
}

[export]
def bench_interp_script(count:int):int {
    var n = 0
    for (i in range(0, count)) {
        if (interp_in_script(double(i), 100.0lf, 0.016lf, 1.0lf) >= 0.0lf) { n += 1 }
    }
    return n
}
