;; DH3
lw	$t1, 0($t0)
lw	$t2, 4($t0)
lw	$t4, 8($t0)
add	$t3, $t1, $t2
sw	$t3, 12($t0)
add	$t5, $t1, $t4
sw	$t5, 16($t0)
end:
b       end
