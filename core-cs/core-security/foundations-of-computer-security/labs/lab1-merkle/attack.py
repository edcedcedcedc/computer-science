class AttackOne:
    def __init__(self, s):
        self._store = s

    def attack_fake_key(self):
        return b"hello"

    def lookup(self, key):
        proof = self._store.lookup(key)
        self.validate(key, proof)

        ## If we found the key, return it; otherwise (empty or other key), None
        if proof.key == key:
            return proof.val
        else:
            return None


class AttackTwo:
    def __init__(self, s):
        self._store = s

    def attack_fake_keys(self):
        return {}

    def attack_key_value(self):
        return b"hello", b"world"

    def lookup(self, key):
        return self._store.lookup(key)


class AttackThree:
    def __init__(self, s):
        self._store = s

    def lookup(self, key):
        return self._store.lookup(key)


class AttackFour:
    def __init__(self, s):
        self._store = s

    def insert(self, key, val):
        return self._store.insert(key, val)

    def attack_fake_key(self):
        return b""

    def lookup(self, key):
        return self._store.lookup(key)
