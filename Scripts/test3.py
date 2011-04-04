import mq2

z = locals().copy()

for x in z:
	print "%020s, %s" % (x, z[x])

	
q = mq2.Character()

print "SpellGem 2: %s" % q.SpellGem(2)


print mq2.TLO.Me.Heading.DegreesCCW()
print mq2.TLO.Spell(254).Name()


def OpenSpellBook():
	if not mq2.TLO.Window["SpellBookWnd"].Open():
		mq2.DoCommand("/keypress spellbook")
		
OpenSpellBook()