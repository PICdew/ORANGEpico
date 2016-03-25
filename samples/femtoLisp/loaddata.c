#include <stdio.h>			// always in first place to avoid conflict with const.h ON
#include <stdarg.h>

const char system_lsp[]=
"; femtoLisp standard library\n"
"; by Jeff Bezanson\n"
"; Public Domain\n"
"\n"
"(set 'list (lambda args args))\n"
"\n"
;
const char system_lspX[]=
"; femtoLisp standard library\n"
"; by Jeff Bezanson\n"
"; Public Domain\n"
"\n"
"(set 'list (lambda args args))\n"
"\n"
"(set 'setq (macro (name val)\n"
"                  (list set (list quote name) val)))\n"
"\n"
"(setq sp '| |)\n"
"(setq nl '|\n"
"|)\n"
"\n"
"; convert a sequence of body statements to a single expression.\n"
"; this allows define, defun, defmacro, let, etc. to contain multiple\n"
"; body expressions as in Common Lisp.\n"
"(setq f-body (lambda (e)\n"
"               (cond ((atom e)        e)\n"
"                     ((eq (cdr e) ()) (car e))\n"
"                     (t               (cons progn e)))))\n"
"\n"
"(setq defmacro\n"
"      (macro (name args . body)\n"
"             (list 'setq name (list 'macro args (f-body body)))))\n"
"\n"
"; support both CL defun and Scheme-style define\n"
"(defmacro defun (name args . body)\n"
"  (list 'setq name (list 'lambda args (f-body body))))\n"
"\n"
"(defmacro define (name . body)\n"
"  (if (symbolp name)\n"
"      (list 'setq name (car body))\n"
"    (cons 'defun (cons (car name) (cons (cdr name) body)))))\n"
"\n"
"(defun identity (x) x)\n"
"(setq null not)\n"
"(defun consp (x) (not (atom x)))\n"
"\n"
"(defun map (f lst)\n"
"  (if (atom lst) lst\n"
"    (cons (f (car lst)) (map f (cdr lst)))))\n"
"\n"
"(defmacro let (binds . body)\n"
"  (cons (list 'lambda (map car binds) (f-body body))\n"
"        (map cadr binds)))\n"
"\n"
"(defun nconc lsts\n"
"  (cond ((null lsts) ())\n"
"        ((null (cdr lsts)) (car lsts))\n"
"        (t ((lambda (l d) (if (null l) d\n"
"                            (prog1 l\n"
"                              (while (consp (cdr l)) (set 'l (cdr l)))\n"
"                              (rplacd l d))))\n"
"            (car lsts) (apply nconc (cdr lsts))))))\n"
"\n"
"(defun append lsts\n"
"  (cond ((null lsts) ())\n"
"        ((null (cdr lsts)) (car lsts))\n"
"        (t ((label append2 (lambda (l d)\n"
"                             (if (null l) d\n"
"                               (cons (car l)\n"
"                                     (append2 (cdr l) d)))))\n"
"            (car lsts) (apply append (cdr lsts))))))\n"
"\n"
"(defun member (item lst)\n"
"  (cond ((atom lst) ())\n"
"        ((eq (car lst) item) lst)\n"
"        (t (member item (cdr lst)))))\n"
"\n"
"(defun macrop (e) (and (consp e) (eq (car e) 'macro) e))\n"
"(defun macrocallp (e) (and (symbolp (car e))\n"
"                           (boundp (car e))\n"
"                           (macrop (eval (car e)))))\n"
"(defun macroapply (m args) (apply (cons 'lambda (cdr m)) args))\n"
"\n"
"(defun macroexpand-1 (e)\n"
"  (if (atom e) e\n"
"    (let ((f (macrocallp e)))\n"
"      (if f (macroapply f (cdr e))\n"
"        e))))\n"
"\n"
"; convert to proper list, i.e. remove \"dots\", and append\n"
"(defun append.2 (l tail)\n"
"  (cond ((null l)  tail)\n"
"        ((atom l)  (cons l tail))\n"
"        (t         (cons (car l) (append.2 (cdr l) tail)))))\n"
"\n"
"(defun macroexpand (e)\n"
"  ((label mexpand\n"
"          (lambda (e env f)\n"
"            (progn\n"
"              (while (and (consp e)\n"
"                          (not (member (car e) env))\n"
"                          (set 'f (macrocallp e)))\n"
"                (set 'e (macroapply f (cdr e))))\n"
"              (if (and (consp e)\n"
"                       (not (or (eq (car e) 'quote)\n"
"                                (eq (car e)  quote))))\n"
"                  (let ((newenv\n"
"                         (if (and (or (eq (car e) 'lambda) (eq (car e) 'macro))\n"
"                                  (consp (cdr e)))\n"
"                             (append.2 (cadr e) env)\n"
"                           env)))\n"
"                    (map (lambda (x) (mexpand x newenv nil)) e))\n"
"                e))))\n"
"   e nil nil))\n"
"\n"
"; uncomment this to macroexpand functions at definition time.\n"
"; makes typical code ~25% faster, but only works for defun expressions\n"
"; at the top level.\n"
";(defmacro defun (name args . body)\n"
";  (list 'setq name (list 'lambda args (macroexpand (f-body body)))))\n"
"\n"
"; same thing for macros. enabled by default because macros are usually\n"
"; defined at the top level.\n"
"(defmacro defmacro (name args . body)\n"
"  (list 'setq name (list 'macro args (macroexpand (f-body body)))))\n"
"\n"
"(setq =   eq)\n"
"(setq eql eq)\n"
"(define (/= a b) (not (eq a b)))\n"
"(define != /=)\n"
"(define (>  a b) (< b a))\n"
"(define (<= a b) (not (< b a)))\n"
"(define (>= a b) (not (< a b)))\n"
"(define (mod x y) (- x (* (/ x y) y)))\n"
"(define (abs x)   (if (< x 0) (- x) x))\n"
"(define (truncate x) x)\n"
"(setq K prog1)  ; K combinator ;)\n"
"(define (funcall f . args) (apply f args))\n"
"(define (symbol-function sym) (eval sym))\n"
"(define (symbol-value    sym) (eval sym))\n"
"\n"
"(define (caar x) (car (car x)))\n"
"(define (cadr x) (car (cdr x)))\n"
"(define (cdar x) (cdr (car x)))\n"
"(define (cddr x) (cdr (cdr x)))\n"
"(define (caaar x) (car (car (car x))))\n"
"(define (caadr x) (car (car (cdr x))))\n"
"(define (cadar x) (car (cdr (car x))))\n"
"(define (caddr x) (car (cdr (cdr x))))\n"
"(define (cdaar x) (cdr (car (car x))))\n"
"(define (cdadr x) (cdr (car (cdr x))))\n"
"(define (cddar x) (cdr (cdr (car x))))\n"
"(define (cdddr x) (cdr (cdr (cdr x))))\n"
"\n"
"(define (equal a b)\n"
"  (if (and (consp a) (consp b))\n"
"      (and (equal (car a) (car b))\n"
"           (equal (cdr a) (cdr b)))\n"
"    (eq a b)))\n"
"\n"
"; compare imposes an ordering on all values. yields -1 for a<b,\n"
"; 0 for a==b, and 1 for a>b. lists are compared up to the first\n"
"; point of difference.\n"
"(defun compare (a b)\n"
"  (cond ((eq a b) 0)\n"
"        ((or (atom a) (atom b)) (if (< a b) -1 1))\n"
"        (t (let ((c (compare (car a) (car b))))\n"
"             (if (not (eq c 0))\n"
"                 c\n"
"               (compare (cdr a) (cdr b)))))))\n"
"\n"
"(defun every (pred lst)\n"
"  (or (atom lst)\n"
"      (and (pred (car lst))\n"
"           (every pred (cdr lst)))))\n"
"\n"
"(defun any (pred lst)\n"
"  (and (consp lst)\n"
"       (or (pred (car lst))\n"
"           (any pred (cdr lst)))))\n"
"\n"
"(defun listp (a) (or (eq a ()) (consp a)))\n"
"\n"
"(defun length (l)\n"
"  (if (null l) 0\n"
"    (+ 1 (length (cdr l)))))\n"
"\n"
"(defun nthcdr (n lst)\n"
"  (if (<= n 0) lst\n"
"    (nthcdr (- n 1) (cdr lst))))\n"
"\n"
"(defun list-ref (lst n)\n"
"  (car (nthcdr n lst)))\n"
"\n"
"(defun list* l\n"
"  (if (atom (cdr l))\n"
"      (car l)\n"
"    (cons (car l) (apply list* (cdr l)))))\n"
"\n"
"(defun nlist* l\n"
"  (if (atom (cdr l))\n"
"      (car l)\n"
"    (rplacd l (apply nlist* (cdr l)))))\n"
"\n"
"(defun lastcdr (l)\n"
"  (if (atom l) l\n"
"    (lastcdr (cdr l))))\n"
"\n"
"(defun last (l)\n"
"  (cond ((atom l)        l)\n"
"        ((atom (cdr l))  l)\n"
"        (t               (last (cdr l)))))\n"
"\n"
"(defun map! (f lst)\n"
"  (prog1 lst\n"
"    (while (consp lst)\n"
"      (rplaca lst (f (car lst)))\n"
"      (set 'lst (cdr lst)))))\n"
"\n"
"(defun mapcar (f . lsts)\n"
"  ((label mapcar-\n"
"          (lambda (lsts)\n"
"            (cond ((null lsts) (f))\n"
"                  ((atom (car lsts)) (car lsts))\n"
"                  (t (cons (apply f (map car lsts))\n"
"                           (mapcar- (map cdr lsts)))))))\n"
"   lsts))\n"
"\n"
"(defun transpose (M) (apply mapcar (cons list M)))\n"
"\n"
"(defun filter (pred lst)\n"
"  (cond ((null lst) ())\n"
"        ((not (pred (car lst))) (filter pred (cdr lst)))\n"
"        (t (cons (car lst) (filter pred (cdr lst))))))\n"
"\n"
"(define (foldr f zero lst)\n"
"  (if (null lst) zero\n"
"    (f (car lst) (foldr f zero (cdr lst)))))\n"
"\n"
"(define (foldl f zero lst)\n"
"  (if (null lst) zero\n"
"    (foldl f (f (car lst) zero) (cdr lst))))\n"
"\n"
"(define (reverse lst) (foldl cons nil lst))\n"
"\n"
"(define (reduce0 f zero lst)\n"
"  (if (null lst) zero\n"
"    (reduce0 f (f zero (car lst)) (cdr lst))))\n"
"\n"
"(defun reduce (f lst)\n"
"  (reduce0 f (car lst) (cdr lst)))\n"
"\n"
"(define (copy-list l) (map identity l))\n"
"(define (copy-tree l)\n"
"  (if (atom l) l\n"
"    (cons (copy-tree (car l))\n"
"          (copy-tree (cdr l)))))\n"
"\n"
"(define (assoc item lst)\n"
"  (cond ((atom lst) ())\n"
"        ((eq (caar lst) item) (car lst))\n"
"        (t (assoc item (cdr lst)))))\n"
"\n"
"(define (nreverse l)\n"
"  (let ((prev nil))\n"
"    (while (consp l)\n"
"      (set 'l (prog1 (cdr l)\n"
"                (rplacd l (prog1 prev\n"
"                            (set 'prev l))))))\n"
"    prev))\n"
"\n"
"(defmacro let* (binds . body)\n"
"  (cons (list 'lambda (map car binds)\n"
"              (cons progn\n"
"                    (nconc (map (lambda (b) (cons 'setq b)) binds)\n"
"                           body)))\n"
"        (map (lambda (x) nil) binds)))\n"
"\n"
"(defmacro labels (binds . body)\n"
"  (cons (list 'lambda (map car binds)\n"
"              (cons progn\n"
"                    (nconc (map (lambda (b)\n"
"                                  (list 'setq (car b) (cons 'lambda (cdr b))))\n"
"                                binds)\n"
"                           body)))\n"
"        (map (lambda (x) nil) binds)))\n"
"\n"
"(defmacro when   (c . body) (list if c (f-body body) nil))\n"
"(defmacro unless (c . body) (list if c nil (f-body body)))\n"
"\n"
"(defmacro dotimes (var . body)\n"
"  (let ((v (car var))\n"
"        (cnt (cadr var)))\n"
"    (list 'let (list (list v 0))\n"
"          (list while (list < v cnt)\n"
"                (list prog1 (f-body body) (list 'setq v (list + v 1)))))))\n"
"\n"
"(defun map-int (f n)\n"
"  (let ((acc nil))\n"
"    (dotimes (i n)\n"
"      (setq acc (cons (f i) acc)))\n"
"    (nreverse acc)))\n"
"\n"
"; property lists\n"
"(setq *plists* nil)\n"
"\n"
"(defun symbol-plist (sym)\n"
"  (cdr (or (assoc sym *plists*) '(()))))\n"
"\n"
"(defun set-symbol-plist (sym lst)\n"
"  (let ((p (assoc sym *plists*)))\n"
"    (if (null p)  ; sym has no plist yet\n"
"        (setq *plists* (cons (cons sym lst) *plists*))\n"
"      (rplacd p lst))))\n"
"\n"
"(defun get (sym prop)\n"
"  (let ((pl (symbol-plist sym)))\n"
"    (if pl\n"
"        (let ((pr (member prop pl)))\n"
"          (if pr (cadr pr) nil))\n"
"      nil)))\n"
"\n"
"(defun put (sym prop val)\n"
"  (let ((p (assoc sym *plists*)))\n"
"    (if (null p)  ; sym has no plist yet\n"
"        (setq *plists* (cons (list sym prop val) *plists*))\n"
"      (let ((pr (member prop p)))\n"
"        (if (null pr)  ; sym doesn't have this property yet\n"
"            (rplacd p (cons prop (cons val (cdr p))))\n"
"          (rplaca (cdr pr) val)))))\n"
"  val)\n"
"\n"
"; setf\n"
"; expands (setf (place x ...) v) to (mutator (f x ...) v)\n"
"; (mutator (identity x ...) v) is interpreted as (mutator x ... v)\n"
"(setq *setf-place-list*\n"
"       ; place   mutator  f\n"
"      '((car     rplaca   identity)\n"
"        (cdr     rplacd   identity)\n"
"        (caar    rplaca   car)\n"
"        (cadr    rplaca   cdr)\n"
"        (cdar    rplacd   car)\n"
"        (cddr    rplacd   cdr)\n"
"        (caaar   rplaca   caar)\n"
"        (caadr   rplaca   cadr)\n"
"        (cadar   rplaca   cdar)\n"
"        (caddr   rplaca   cddr)\n"
"        (cdaar   rplacd   caar)\n"
"        (cdadr   rplacd   cadr)\n"
"        (cddar   rplacd   cdar)\n"
"        (cdddr   rplacd   cddr)\n"
"        (get     put      identity)\n"
"        (aref    aset     identity)\n"
"        (symbol-function   set                identity)\n"
"        (symbol-value      set                identity)\n"
"        (symbol-plist      set-symbol-plist   identity)))\n"
"\n"
"(defun setf-place-mutator (place val)\n"
"  (if (symbolp place)\n"
"      (list 'setq place val)\n"
"    (let ((mutator (assoc (car place) *setf-place-list*)))\n"
"      (if (null mutator)\n"
"          (error '|setf: error: unknown place | (car place))\n"
"        (if (eq (caddr mutator) 'identity)\n"
"            (cons (cadr mutator) (append (cdr place) (list val)))\n"
"          (list (cadr mutator)\n"
"                (cons (caddr mutator) (cdr place))\n"
"                val))))))\n"
"\n"
"(defmacro setf args\n"
"  (f-body\n"
"   ((label setf-\n"
"           (lambda (args)\n"
"             (if (null args)\n"
"                 nil\n"
"               (cons (setf-place-mutator (car args) (cadr args))\n"
"                     (setf- (cddr args))))))\n"
"    args)))\n"
"\n"
"(defun revappend (l1 l2) (nconc (reverse l1) l2))\n"
"(defun nreconc   (l1 l2) (nconc (nreverse l1) l2))\n"
"\n"
"(defun builtinp (x)\n"
"  (and (atom x)\n"
"       (not (symbolp x))\n"
"       (not (numberp x))))\n"
"\n"
"(defun self-evaluating-p (x)\n"
"  (or (eq x nil)\n"
"      (eq x t)\n"
"      (and (atom x)\n"
"           (not (symbolp x)))))\n"
"\n"
"; backquote\n"
"(defmacro backquote (x) (bq-process x))\n"
"\n"
"(defun splice-form-p (x)\n"
"  (or (and (consp x) (or (eq (car x) '*comma-at*)\n"
"                         (eq (car x) '*comma-dot*)))\n"
"      (eq x '*comma*)))\n"
"\n"
"(defun bq-process (x)\n"
"  (cond ((self-evaluating-p x)        x)\n"
"        ((atom x)                     (list quote x))\n"
"        ((eq (car x) 'backquote)      (bq-process (bq-process (cadr x))))\n"
"        ((eq (car x) '*comma*)        (cadr x))\n"
"        ((not (any splice-form-p x))\n"
"         (let ((lc    (lastcdr x))\n"
"               (forms (map bq-bracket1 x)))\n"
"           (if (null lc)\n"
"               (cons 'list forms)\n"
"             (nconc (cons 'nlist* forms) (list (bq-process lc))))))\n"
"        (t (let ((p x) (q '()))\n"
"             (while (and (consp p)\n"
"                         (not (eq (car p) '*comma*)))\n"
"               (setq q (cons (bq-bracket (car p)) q))\n"
"               (setq p (cdr p)))\n"
"             (cons 'nconc\n"
"                   (cond ((consp p) (nreconc q (list (cadr p))))\n"
"                         ((null p)  (nreverse q))\n"
"                         (t         (nreconc q (list (bq-process p))))))))))\n"
"\n"
"(defun bq-bracket (x)\n"
"  (cond ((atom x)                   (list cons (bq-process x) nil))\n"
"        ((eq (car x) '*comma*)      (list cons (cadr x)       nil))\n"
"        ((eq (car x) '*comma-at*)   (list 'copy-list (cadr x)))\n"
"        ((eq (car x) '*comma-dot*)  (cadr x))\n"
"        (t                          (list cons (bq-process x) nil))))\n"
"\n"
"; bracket without splicing\n"
"(defun bq-bracket1 (x)\n"
"  (if (and (consp x) (eq (car x) '*comma*))\n"
"      (cadr x)\n"
"    (bq-process x)))\n"
"\n"

;

/********************************************************************
 *	
 ********************************************************************
 */
